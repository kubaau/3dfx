#pragma once

#include <vector>
#include "DXMathTypedefs.hpp"

#define ALIGN(n) __declspec(align(n))
constexpr auto light_alignment = sizeof(float4);

struct BaseLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
};
static_assert(sizeof(BaseLight) % light_alignment == 0, "");

struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 direction;
    float pad;
};
static_assert(sizeof(DirectionalLight) % light_alignment == 0, "");
using DirectionalLights = vector<DirectionalLight>;

struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 pos;
    float range;

    float3 attenuation;
    float pad;
};
static_assert(sizeof(PointLight) % light_alignment == 0, "");
using PointLights = vector<PointLight>;

struct SpotLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 direction;
    float pad;
    
    float3 pos;
    float range;

    float3 attenuation;
    float spot;
};
static_assert(sizeof(SpotLight) % light_alignment == 0, "");
using SpotLights = vector<SpotLight>;
