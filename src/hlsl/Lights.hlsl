#ifndef Lights_hlsl
#define Lights_hlsl

#include "Material.hlsl"

struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 direction;
    float pad;
};

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

struct LightColor
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
};

static const uint max_dir_lights = 10;
static const uint max_point_lights = 3;
static const uint max_spot_lights = max_point_lights;

float toonShade(float diffuse_factor)
{
    if (diffuse_factor < 0.0f)
        diffuse_factor = 0.4f;
    else if (diffuse_factor <= 0.5f)
        diffuse_factor = 0.6f;
    else
        diffuse_factor = 1.0f;

    return diffuse_factor;
}

LightColor directionalLight(Material mat, DirectionalLight light, float3 normal, float3 to_eye)
{
    LightColor light_color;
    light_color.ambient = mat.ambient * light.ambient;

    const float3 light_v = -light.direction;
    float diffuse_factor = dot(light_v, normal);
    // diffuse_factor = toonShade(diffuse_factor);

    [flatten] if (diffuse_factor > 0)
    {
        light_color.diffuse = diffuse_factor * mat.diffuse * light.diffuse;

        const float3 reflection = reflect(light.direction, normal);
        const float specular_factor = pow(max(dot(reflection, to_eye), 0.0f), mat.specular.w);
        light_color.specular = specular_factor * mat.specular * light.specular;
    }
    else
    {
        light_color.diffuse = 0;
        light_color.specular = 0;
    }

    return light_color;
}

LightColor pointLight(Material mat, PointLight light, float3 pos, float3 normal, float3 to_eye)
{
    LightColor light_color;
    light_color.ambient = mat.ambient * light.ambient;
    light_color.diffuse = 0;
    light_color.specular = 0;

    float3 light_v = light.pos - pos;
    const float distance = length(light_v);

    [flatten] if (distance <= light.range)
    {
        light_v /= distance;

        const float diffuse_factor = dot(light_v, normal);

        [flatten] if (diffuse_factor > 0.0f)
        {
            light_color.diffuse = diffuse_factor * mat.diffuse * light.diffuse;

            const float3 reflection = reflect(-light_v, normal);
            const float specular_factor = pow(max(dot(reflection, to_eye), 0.0f), mat.specular.w);
            light_color.specular = specular_factor * mat.specular * light.specular;

            const float attenuation = 1.0f / dot(light.attenuation, float3(1.0f, distance, distance * distance));
            light_color.diffuse *= attenuation;
            light_color.specular *= attenuation;
        }
    }

    return light_color;
}

LightColor spotLight(Material mat, SpotLight light, float3 pos, float3 normal, float3 to_eye)
{
    LightColor light_color;
    light_color.ambient = mat.ambient * light.ambient;
    light_color.diffuse = 0;
    light_color.specular = 0;

    float3 light_v = light.pos - pos;
    const float distance = length(light_v);

    [flatten] if (distance <= light.range)
    {
        light_v /= distance;

        const float diffuse_factor = dot(light_v, normal);

        [flatten] if (diffuse_factor > 0.0f)
        {
            light_color.diffuse = diffuse_factor * mat.diffuse * light.diffuse;

            const float3 reflection = reflect(-light_v, normal);
            const float specular_factor = pow(max(dot(reflection, to_eye), 0.0f), mat.specular.w);
            light_color.specular = specular_factor * mat.specular * light.specular;

            const float spot = pow(max(dot(-light_v, light.direction), 0.0f), light.spot);
            const float attenuation = spot / dot(light.attenuation, float3(1.0f, distance, distance * distance));

            light_color.ambient *= spot;
            light_color.diffuse *= attenuation;
            light_color.specular *= attenuation;
        }
        else
        {
            // Scale by spotlight factor and attenuate.
            const float spot = pow(max(dot(-light_v, light.direction), 0.0f), light.spot);
            light_color.ambient *= spot;
        }
    }

    return light_color;
}

void addDirLights(inout float4 ambient,
                  inout float4 diffuse,
                  inout float4 specular,
                  DirectionalLight lights[max_dir_lights],
                  uint light_count,
                  Material mat,
                  float3 normal,
                  float3 to_eye)
{
    for (uint i = 0; i < light_count; ++i)
    {
        const LightColor c = directionalLight(mat, lights[i], normal, to_eye);
        ambient += c.ambient;
        diffuse += c.diffuse;
        specular += c.specular;
    }
}

void addPointLights(inout float4 ambient,
                    inout float4 diffuse,
                    inout float4 specular,
                    PointLight lights[max_point_lights],
                    uint light_count,
                    Material mat,
                    float3 normal,
                    float3 to_eye,
                    float3 pos)
{
    for (uint i = 0; i < light_count; ++i)
    {
        const LightColor c = pointLight(mat, lights[i], pos, normal, to_eye);
        ambient += c.ambient;
        diffuse += c.diffuse;
        specular += c.specular;
    }
}

void addSpotLights(inout float4 ambient,
                   inout float4 diffuse,
                   inout float4 specular,
                   SpotLight lights[max_spot_lights],
                   uint light_count,
                   Material mat,
                   float3 normal,
                   float3 to_eye,
                   float3 pos)
{
    for (uint i = 0; i < light_count; ++i)
    {
        const LightColor c = spotLight(mat, lights[i], pos, normal, to_eye);
        ambient += c.ambient;
        diffuse += c.diffuse;
        specular += c.specular;
    }
}

#endif
