#ifndef ConstantBuffers_hlsl
#define ConstantBuffers_hlsl

#include "Lights.hlsl"
#include "Material.hlsl"

cbuffer per_frame : register(b0)
{
    DirectionalLight dir_lights[max_dir_lights];
    PointLight point_lights[max_point_lights];
    SpotLight spot_lights[max_spot_lights];

    uint dir_lights_count;
    uint point_lights_count;
    uint spot_lights_count;

    float3 eye_pos;
};

cbuffer per_object : register(b1)
{
    float4x4 w;
    float4x4 wit;
    float4x4 wvp;
    float4x4 diffuse_transform;
    Material mat;
};

cbuffer rarely : register(b2)
{
    float4 fog_color;
    float fog_start;
    float fog_range;
};

#endif
