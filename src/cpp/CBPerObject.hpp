#pragma once

#include "DXMaterial.hpp"
#include "DXMathTypedefs.hpp"

struct CBPerObject
{
    float4x4 w;
    float4x4 wit;
    float4x4 wvp;
    float4x4 diffuse_transform;
    Material mat;
};
