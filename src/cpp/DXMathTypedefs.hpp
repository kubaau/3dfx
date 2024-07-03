#pragma once

#include <DirectXMath.h>

using namespace DirectX;

using uint = unsigned;

using float2 = XMFLOAT2;
using float3 = XMFLOAT3;
using float4 = XMFLOAT4;

using float4x4 = XMFLOAT4X4;

using Matrix = DirectX::XMMATRIX;

using Rotation = float3;
using Scale = float3;
using Translation = float3;

using Transform = float4x4;

constexpr auto pi = XM_PI;
