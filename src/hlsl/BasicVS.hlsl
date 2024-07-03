#include "ConstantBuffers.hlsl"
#include "Utils.hlsl"
#include "VSStructs.hlsl"

typedef VInBasic VIn;
typedef VOutBasic VOut;

VOut main(VIn vi)
{
    VOut vo;
    const float4 pos_lh = homogenize(vi.pos_l);
    vo.pos_h = mul(pos_lh, wvp);
    vo.pos_w = mul(pos_lh, w).xyz;
    vo.normal_w = mul(vi.normal_l, (float3x3) wit);
    vo.tex = mul(float4(vi.tex, 0.0f, 1.0f), diffuse_transform).xy;
    return vo;
}
