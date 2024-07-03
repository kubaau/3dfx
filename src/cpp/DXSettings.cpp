#include "DXSettings.hpp"
#include <tools/ArraySize.hpp>
#include "DXCore.hpp"
#include "DXDebugUtils.hpp"
#include "ReadFile.hpp"

DXSettings::DXSettings(DXCore& core) : core(core)
{
    initShaders();
    initRenderStates();
    initConstantBuffers();
}

void DXSettings::initShaders()
{
    VertexShaderPtr vs;
    auto shader = readBinaryFile("BasicVS.cso");
    HR(core.device->CreateVertexShader(shader.data(), shader.size(), nullptr, vs.GetAddressOf()));
    vertex_shaders.emplace("basic", vs);

    const D3D11_INPUT_ELEMENT_DESC basic[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}};
    InputLayoutPtr il;
    HR(core.device->CreateInputLayout(basic, arraySize<unsigned>(basic), shader.data(), shader.size(), &il));
    input_layouts.emplace("basic", il);

    PixelShaderPtr ps;
    shader = readBinaryFile("BasicPS.cso");
    HR(core.device->CreatePixelShader(shader.data(), shader.size(), nullptr, ps.GetAddressOf()));
    pixel_shaders.emplace("basic", ps);

    shader = readBinaryFile("BasicPSNoTexNoClipNoFog.cso");
    HR(core.device->CreatePixelShader(shader.data(), shader.size(), nullptr, ps.GetAddressOf()));
    pixel_shaders.emplace("basic_no_tex", ps);
}

void DXSettings::initRenderStates()
{
    initRStates();
    initBStates();
    initDSStates();
    initSStates();
}

void DXSettings::initRStates()
{
    RStatePtr s;

    D3D11_RASTERIZER_DESC desc{};
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_BACK;
    desc.DepthClipEnable = true;
    HR(core.device->CreateRasterizerState(&desc, &s));
    render_states.emplace("default", s);

    desc.FrontCounterClockwise = true;
    HR(core.device->CreateRasterizerState(&desc, &s));
    render_states.emplace("cull_clockwise", s);

    desc.FillMode = D3D11_FILL_WIREFRAME;
    desc.FrontCounterClockwise = false;
    HR(core.device->CreateRasterizerState(&desc, &s));
    render_states.emplace("wireframe", s);

    desc.FrontCounterClockwise = true;
    HR(core.device->CreateRasterizerState(&desc, &s));
    render_states.emplace("wireframe_cull_clockwise", s);

    desc.FillMode = D3D11_FILL_SOLID;
    desc.FrontCounterClockwise = false;
    desc.ScissorEnable = true;
    HR(core.device->CreateRasterizerState(&desc, &s));
    render_states.emplace("scissor", s);

    desc.CullMode = D3D11_CULL_NONE;
    desc.ScissorEnable = false;
    HR(core.device->CreateRasterizerState(&desc, &s));
    render_states.emplace("no_cull", s);
}

void DXSettings::initDSStates()
{
    DSStatePtr s;

    D3D11_DEPTH_STENCIL_DESC desc{};
    desc.DepthEnable = true;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc.DepthFunc = D3D11_COMPARISON_LESS;

    HR(core.device->CreateDepthStencilState(&desc, &s));
    ds_states.emplace("default", s);
       
    desc.StencilEnable = true;
    desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    // We are not rendering backfacing polygons, so these settings do not matter.
    desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    desc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    
    HR(core.device->CreateDepthStencilState(&desc, &s));
    ds_states.emplace("draw_reflection", s);
}

void DXSettings::initBStates()
{
    BStatePtr s;

    D3D11_BLEND_DESC desc{};
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HR(core.device->CreateBlendState(&desc, &s));
    blend_states.emplace("default", s);

    desc.RenderTarget[0].BlendEnable = true;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    HR(core.device->CreateBlendState(&desc, &s));
    blend_states.emplace("transparent", s);
}

void DXSettings::initSStates()
{
    SStatePtr s;

    D3D11_SAMPLER_DESC desc{};
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.MinLOD = numeric_limits<float>::min();
    desc.MaxLOD = numeric_limits<float>::max();
    HR(core.device->CreateSamplerState(&desc, &s));
    sampler_states.emplace_back(s);

    desc.Filter = D3D11_FILTER_ANISOTROPIC;
    desc.MaxAnisotropy = 4;
    HR(core.device->CreateSamplerState(&desc, &s));
    sampler_states.emplace_back(s);

    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    HR(core.device->CreateSamplerState(&desc, &s));
    sampler_states.emplace_back(s);

    desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    HR(core.device->CreateSamplerState(&desc, &s));
    sampler_states.emplace_back(s);

    desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.ComparisonFunc = D3D11_COMPARISON_LESS;
    HR(core.device->CreateSamplerState(&desc, &s));
    sampler_states.emplace_back(s);

    core.context->PSSetSamplers(0, static_cast<unsigned>(sampler_states.size()), sampler_states.front().GetAddressOf());
}

void DXSettings::initConstantBuffers()
{
    cb_per_frame.init(*core.device);
    cb_per_object.init(*core.device);
    cb_rarely.init(*core.device);

    vector<D3DBuffer*> cbuffers = {cb_per_frame.buffer.Get(), cb_per_object.buffer.Get(), cb_rarely.buffer.Get()};
    const auto size = static_cast<unsigned>(cbuffers.size());
    core.context->VSSetConstantBuffers(0, size, cbuffers.data());
    core.context->PSSetConstantBuffers(0, size, cbuffers.data());

    cb_rarely.apply(*core.context);
}
