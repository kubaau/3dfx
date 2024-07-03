#pragma once

#include "DXAssets.hpp"
#include "DXCore.hpp"
#include "DXScene.hpp"
#include "DXSettings.hpp"
#include "GraphicsEngine.hpp"

class DXEngine : public GraphicsEngine
{
public:
    DXEngine(WindowPtr);

    void run() override;
    void onResize() override;

    void orbit(float delta_theta, float delta_phi) override;
    void zoom(float delta_radius) override;

private:
    DXCore core;
    DXSettings settings;
    DXAssets assets;
    DXScene scene;
};
