#pragma once

#include "GraphicsTypedefs.hpp"
#include "Interface.hpp"

class GraphicsEngine : public Interface
{
public:
    virtual void run() = 0;
    virtual void onResize() = 0;

    virtual void orbit(float delta_theta, float delta_phi) = 0;
    virtual void zoom(float delta_radius) = 0;
};

extern GraphicsEnginePtr graphics_engine;
