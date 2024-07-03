#pragma once

#include "DXColor.hpp"
#include "DXLights.hpp"
#include "DXTypedefs.hpp"

struct DXCore;
struct DXSettings;
struct DXAssets;

struct DXScene
{
    DXScene(DXCore&, DXSettings&, DXAssets&);

    void onResize();
    void draw();

    void orbit(float delta_theta, float delta_phi);
    void zoom(float delta_radius);

private:
    void initScene();
    void initLights();

    void updateProjection();
    void updateViewProjection();

    void update();
    void updateBackgroundColor();
    void updateEye();
    void updateView();
    void updateLights();
    void handleKeyboard();

    void clearViews();

    void drawSolid(const Matrix& vp);
    void drawReflectedSolid(const Matrix& vp);
    void drawTransparent(const Matrix& vp);

    DXCore& core;
    DXSettings& settings;
    DXAssets& assets;

    RState* rs_override{};

    float4x4 view;
    float4x4 projection;
    float4x4 view_projection;

    Renderables renderables;
    DirectionalLights dir_lights;
    PointLights point_lights;
    SpotLights spot_lights;

    Color background_color;

    float theta = 1.5f * pi;
    float phi = 0.1f * pi;
    float radius = 15;
};
