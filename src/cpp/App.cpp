#include "App.hpp"
#include <tools/ThreadSafeLogger.hpp>
#include "GraphicsEngine.hpp"
#include "Timer.hpp"
#include "Window.hpp"

void App::pause()
{
    if (not paused)
    {
        FUNCTION_LOG;
        timer.pause();
        paused = true;
    }
}

void App::unpause()
{
    FUNCTION_LOG;
    timer.unpause();
    graphics_engine->onResize();
    paused = false;
}
