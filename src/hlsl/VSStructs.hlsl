#ifndef VSStructs_hlsl
#define VSStructs_hlsl

struct VInBasic
{
    float3 pos_l : POSITION;
    float3 normal_l : NORMAL;
    float2 tex : TEXCOORD;
};

struct VOutBasic
{
    float4 pos_h : SV_POSITION;
    float3 pos_w : POSITION;
    float3 normal_w : NORMAL;
    float2 tex : TEXCOORD0;
};

#endif
