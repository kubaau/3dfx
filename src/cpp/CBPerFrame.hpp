#pragma once

#include <array>
#include "DXLights.hpp"
#include "DXMathTypedefs.hpp"

constexpr auto max_dir_lights = 10;
constexpr auto max_point_lights = 3;
constexpr auto max_spot_lights = max_point_lights;

struct CBPerFrame
{
    array<DirectionalLight, max_dir_lights> dir_lights;
    array<PointLight, max_point_lights> point_lights;
    array<SpotLight, max_spot_lights> spot_lights;
    
    uint dir_lights_count;
    uint point_lights_count;
    uint spot_lights_count;

    float3 eye_pos;
};
