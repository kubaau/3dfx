#pragma once

#include "DXMathTypedefs.hpp"

struct VertexPNT
{
    VertexPNT(float3 p = {0, 0, 0}, float3 n = {0, 0, 0}, float2 uv = {0, 0})
        : pos(move(p)), normal(move(n)), tex(move(uv))
    {
    }

    VertexPNT(float px, float py, float pz, float nx, float ny, float nz, float u, float v)
        : pos(px, py, pz), normal(nx, ny, nz), tex(u, v)
    {
    }

    float3 pos;
    float3 normal;
    float2 tex;
};
