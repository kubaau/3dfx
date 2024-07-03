#include "DXCore.hpp"
#include <tools/ThreadSafeLogger.hpp>
#include "CheckResult.hpp"
#include "DXDebugUtils.hpp"
#include "Window.hpp"

DXCore::DXCore(WindowPtr window) : window(move(window))
{
    initContext();
    initSwapChain();
}

void DXCore::onResize()
{
    releaseOldComs();
    updateSwapChain();
    updateRtv();
    updateDsb();
    updateDsv();
    updateViewport();
    updateOffscreenViews();
}

void DXCore::initContext()
{
    auto flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    constexpr auto driver_type = D3D_DRIVER_TYPE_HARDWARE;

    D3D_FEATURE_LEVEL feature_level;
    const auto device_result = D3D11CreateDevice(0, // default adapter
                                                 driver_type,
                                                 0, // no software device
                                                 flags,
                                                 0,
                                                 0, // default feature level array
                                                 D3D11_SDK_VERSION,
                                                 &device,
                                                 &feature_level,
                                                 &context);

    check(not FAILED(device_result), "D3D11CreateDevice failed");
    check(feature_level == D3D_FEATURE_LEVEL_11_0, "Direct3D Feature Level 11 unsupported");

    // Check 4X MSAA quality support for our back buffer format.
    // All Direct3D 11 capable devices support 4X MSAA for all render target formats, so we only need to check quality
    // support.
    HR(device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_4x_msaa_quality));
}

namespace
{
    constexpr auto enable_4x_msaa = true;

    void disableAltEnterFullscreen(IDXGIFactory& dxgi_factory, Window& w)
    {
        HR(dxgi_factory.MakeWindowAssociation(reinterpret_cast<HWND>(w.getHandle()), DXGI_MWA_NO_WINDOW_CHANGES));
    }

    void debugAdapters(IDXGIFactory& dxgi_factory)
    {
        int adapter_count = 0;
        ComPtr<IDXGIAdapter> adapter;
        while (dxgi_factory.EnumAdapters(adapter_count, &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            ++adapter_count;
            int output_count = 0;
            ComPtr<IDXGIOutput> output;
            while (adapter->EnumOutputs(output_count, &output) != DXGI_ERROR_NOT_FOUND)
            {
                ++output_count;

                unsigned mode_count;
                HR(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &mode_count, nullptr));
                vector<DXGI_MODE_DESC> mode_descs(mode_count);
                HR(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &mode_count, mode_descs.data()));
                for (const auto& desc : mode_descs)
                    DEBUG_LOG << desc.Width << "x" << desc.Height << " "
                              << static_cast<float>(desc.RefreshRate.Numerator) / desc.RefreshRate.Denominator << "Hz";
            }
            DEBUG_LOG_VAR(adapter_count) DEBUG_VAR(output_count);
        }
    }
}

void DXCore::initSwapChain()
{
    DXGI_SWAP_CHAIN_DESC desc;
    desc.BufferDesc.Width = window->getWidth();
    desc.BufferDesc.Height = window->getHeight();
    desc.BufferDesc.RefreshRate.Numerator = 144;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.SampleDesc.Count = enable_4x_msaa ? 4 : 1;
    desc.SampleDesc.Quality = enable_4x_msaa ? _4x_msaa_quality - 1 : 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 1;
    desc.OutputWindow = reinterpret_cast<HWND>(window->getHandle());
    desc.Windowed = true;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    desc.Flags = 0;

    // To correctly create the swap chain, we must use the IDXGIFactory that was used to create the device.
    // If we tried to use a different IDXGIFactory instance (by calling CreateDXGIFactory), we get an error:
    // "IDXGIFactory::CreateSwapChain: This function is being called with a device from a different IDXGIFactory."
    ComPtr<IDXGIDevice> dxgi_device;
    HR(device->QueryInterface(__uuidof(IDXGIDevice), &dxgi_device));

    ComPtr<IDXGIAdapter> dxgi_adapter;
    HR(dxgi_device->GetParent(__uuidof(IDXGIAdapter), &dxgi_adapter));

    ComPtr<IDXGIFactory> dxgi_factory;
    HR(dxgi_adapter->GetParent(__uuidof(IDXGIFactory), &dxgi_factory));

    HR(dxgi_factory->CreateSwapChain(device.Get(), &desc, &swap_chain));

    debugAdapters(*dxgi_factory);
}

void DXCore::releaseOldComs()
{
    rtv.Reset();
    dsv.Reset();
    dsb.Reset();
    offscreen_srv.Reset();
    offscreen_uav.Reset();
    offscreen_rtv.Reset();
}

void DXCore::updateSwapChain()
{
    HR(swap_chain->ResizeBuffers(1, window->getWidth(), window->getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 0));
}

void DXCore::updateRtv()
{
    Texture2DPtr back_buffer;
    HR(swap_chain->GetBuffer(0, __uuidof(Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf())));
    HR(device->CreateRenderTargetView(back_buffer.Get(), 0, &rtv));
}

void DXCore::updateDsb()
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = window->getWidth();
    desc.Height = window->getHeight();
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count = enable_4x_msaa ? 4 : 1;
    desc.SampleDesc.Quality = enable_4x_msaa ? _4x_msaa_quality - 1 : 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    HR(device->CreateTexture2D(&desc, 0, &dsb));
}

void DXCore::updateDsv()
{
    HR(device->CreateDepthStencilView(dsb.Get(), 0, &dsv));
}

void DXCore::updateViewport()
{
    viewport.Width = static_cast<float>(window->getWidth());
    viewport.Height = static_cast<float>(window->getHeight());
    context->RSSetViewports(1, &viewport);
}

void DXCore::updateOffscreenViews()
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = window->getWidth();
    desc.Height = window->getHeight();
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    Texture2DPtr tex;
    HR(device->CreateTexture2D(&desc, 0, &tex));

    // Null description means to create a view to all mipmap levels using the format the texture was created with.
    HR(device->CreateShaderResourceView(tex.Get(), 0, &offscreen_srv));
    HR(device->CreateUnorderedAccessView(tex.Get(), 0, &offscreen_uav));
    HR(device->CreateRenderTargetView(tex.Get(), 0, &offscreen_rtv));
}
