#pragma once

#include <d3d11.h>
#include <tools/ComPtr.hpp>
#include "ContainerTypedefs.hpp"

using Device = ID3D11Device;
using DevicePtr = ComPtr<Device>;

using Context = ID3D11DeviceContext;
using ContextPtr = ComPtr<Context>;

using SwapChain = IDXGISwapChain;
using SwapChainPtr = ComPtr<SwapChain>;

using RTV = ID3D11RenderTargetView;
using RTVPtr = ComPtr<RTV>;

using Texture2D = ID3D11Texture2D;
using Texture2DPtr = ComPtr<Texture2D>;

using DSV = ID3D11DepthStencilView;
using DSVPtr = ComPtr<DSV>;

using Viewport = D3D11_VIEWPORT;

using SRV = ID3D11ShaderResourceView;
using SRVPtr = ComPtr<SRV>;
using SRVs = Map<SRVPtr>;
using Texture = SRVPtr;
using Textures = Map<Texture>;

using UAV = ID3D11UnorderedAccessView;
using UAVPtr = ComPtr<UAV>;

using D3DBuffer = ID3D11Buffer;
using D3DBufferPtr = ComPtr<D3DBuffer>;

using InputLayout = ID3D11InputLayout;
using InputLayoutPtr = ComPtr<InputLayout>;
using InputLayouts = Map<InputLayoutPtr>;

using RState = ID3D11RasterizerState;
using RStatePtr = ComPtr<RState>;
using RStates = Map<RStatePtr>;

using DSState = ID3D11DepthStencilState;
using DSStatePtr = ComPtr<DSState>;
using DSStates = Map<DSStatePtr>;

using BState = ID3D11BlendState;
using BStatePtr = ComPtr<BState>;
using BStates = Map<BStatePtr>;

using SState = ID3D11SamplerState;
using SStatePtr = ComPtr<SState>;
using SStates = vector<SStatePtr>;

using VertexShader = ID3D11VertexShader;
using VertexShaderPtr = ComPtr<VertexShader>;
using VertexShaders = Map<VertexShaderPtr>;

using PixelShader = ID3D11PixelShader;
using PixelShaderPtr = ComPtr<PixelShader>;
using PixelShaders = Map<PixelShaderPtr>;

using DXResource = ID3D11Resource;
using DXResourcePtr = ComPtr<DXResource>;

using Topology = D3D_PRIMITIVE_TOPOLOGY;

using Meshes = Map<class Mesh>;
using Materials = Map<struct Material>;
using Renderables = Map<class Renderable>;
