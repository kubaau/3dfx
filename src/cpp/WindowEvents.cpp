#include "WindowEvents.hpp"
#include <tools/ThreadSafeLogger.hpp>
#include "App.hpp"
#include "DXMathTypedefs.hpp"
#include "GraphicsEngine.hpp"
#include "Window.hpp"

#define FLOGP FUNCTION_LOG << p

namespace
{
    bool resizing = false;
}

void ActivatedWindowEvent::handle()
{
    FUNCTION_LOG;
    app.unpause();
}

void DeactivatedWindowEvent::handle()
{
    FUNCTION_LOG;
    app.pause();
}

void MinimizedWindowEvent::handle()
{
    FUNCTION_LOG;
    app.pause();
}

void MaximizedWindowEvent::handle()
{
    FUNCTION_LOG;
    app.unpause();
}

void RestoredWindowEvent::handle()
{
    FUNCTION_LOG;
    if (not resizing)
        app.unpause();
}

void ResizingStartedWindowEvent::handle()
{
    FUNCTION_LOG;
    resizing = true;
    app.pause();
}

void ResizingStoppedWindowEvent::handle()
{
    FUNCTION_LOG;
    resizing = false;
    app.unpause();
}

namespace
{
    Point last_mouse_pos{};
    bool orbiting = false;
    bool zooming = false;
} // namespace

void LmbDownEvent::handle()
{
    DEBUG_LOG << p;
    last_mouse_pos = p;
    orbiting = true;
    window->capture();
}

void LmbUpEvent::handle()
{
    DEBUG_LOG << p;
    orbiting = false;
    window->releaseCapture();
}

void MmbDownEvent::handle()
{
    DEBUG_LOG << p;
}

void MmbUpEvent::handle()
{
    DEBUG_LOG << p;
    window->releaseCapture();
}

void RmbDownEvent::handle()
{
    DEBUG_LOG << p;
    last_mouse_pos = p;
    zooming = true;
}

void RmbUpEvent::handle()
{
    DEBUG_LOG << p;
    zooming = false;
    window->releaseCapture();
}

void MouseMovedEvent::handle()
{
    if (orbiting)
    {
        // each pixel corresponds to a quarter of a degree
        graphics_engine->orbit(XMConvertToRadians(0.25f * (p.x - last_mouse_pos.x)),
                               XMConvertToRadians(0.25f * (p.y - last_mouse_pos.y)));
    }
    if (zooming)
    {
        // each pixel corresponds to 0.01 unit in the scene
        const auto dx = 0.01f * (p.x - last_mouse_pos.x);
        const auto dy = 0.01f * (p.y - last_mouse_pos.y);
        graphics_engine->zoom(dx - dy);
    }

    last_mouse_pos = p;
}
