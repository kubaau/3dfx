#ifndef Utils_hlsl
#define Utils_hlsl

float4 homogenize(float3 v)
{
    return float4(v, 1.0f);
}

struct ToEye
{
    float3 v;
    float d;
};

ToEye toEye(float3 eye_pos, float3 obj_pos)
{
    ToEye to_eye;
    to_eye.v = eye_pos - obj_pos;
    to_eye.d = length(to_eye.v);
    to_eye.v /= to_eye.d;
    return to_eye;
}

void addFogColor(inout float4 p, float distance, float fog_start, float fog_range, float4 fog_color)
{
    const float fog_lerp = saturate((distance - fog_start) / fog_range);
    p = lerp(p, fog_color, fog_lerp);
}

#endif
