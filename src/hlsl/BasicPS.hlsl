#include "ConstantBuffers.hlsl"
#include "States.hlsl"
#include "Utils.hlsl"
#include "VSStructs.hlsl"

#ifndef USE_TEXTURE
#define USE_TEXTURE 1
#endif

#ifndef ALPHA_CLIP
#define ALPHA_CLIP 1
#endif

#ifndef FOG_ENABLED
#define FOG_ENABLED 1
#endif

typedef VOutBasic PIn;

Texture2D diffuse_map : register(t0);

float4 main(PIn pi) : SV_Target
{
#if USE_TEXTURE
    float4 tex_color = diffuse_map.Sample(anisotropic_sampler, pi.tex);
#if ALPHA_CLIP
    clip(tex_color.a - 0.1f);
#endif
#else
    float4 tex_color = mat.diffuse;
#endif

    pi.normal_w = normalize(pi.normal_w);
    const ToEye to_eye = toEye(eye_pos, pi.pos_w);

    float4 ambient = 0;
    float4 diffuse = 0;
    float4 specular = 0;
    addDirLights(ambient, diffuse, specular, dir_lights, dir_lights_count, mat, pi.normal_w, to_eye.v);
    addPointLights(ambient, diffuse, specular, point_lights, point_lights_count, mat, pi.normal_w, to_eye.v, pi.pos_w);
    addSpotLights(ambient, diffuse, specular, spot_lights, spot_lights_count, mat, pi.normal_w, to_eye.v, pi.pos_w);

    float4 pixel_color = tex_color * (ambient + diffuse) + specular;

#if FOG_ENABLED
    addFogColor(pixel_color, to_eye.d, fog_start, fog_range, fog_color);
#endif

    pixel_color.a = mat.diffuse.a * tex_color.a;

    return pixel_color;
}
