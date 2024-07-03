#include "DXEngine.hpp"
#include "DXMesh.hpp"
#include "DXRenderable.hpp"

DXEngine::DXEngine(WindowPtr window)
    : core(move(window)), settings(core), assets(core, settings), scene(core, settings, assets)
{
    onResize();
}

void DXEngine::run()
{
    scene.draw();
}

void DXEngine::onResize()
{
    core.onResize();
    scene.onResize();
}

void DXEngine::orbit(float delta_theta, float delta_phi)
{
    scene.orbit(delta_theta, delta_phi);
}

void DXEngine::zoom(float delta_radius)
{
    scene.zoom(delta_radius);
}
