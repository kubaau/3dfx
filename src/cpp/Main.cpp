#include "Main.hpp"
#include <tools/ThreadSafeLogger.hpp>
#include "App.hpp"
#include "AppMainLoop.hpp"
#include "GraphicsEngine.hpp"
#include "GraphicsEngineFactory.hpp"
#include "Window.hpp"
#include "WindowFactory.hpp"

void runMain(WindowFactoryPtr window_factory,
             GraphicsEngineFactoryPtr graphics_engine_factory,
             OsMsgHandler& os_msg_handler,
             OsEnableDebugToolsFunc enableDebugTools) try
{
#ifdef _DEBUG
    enableDebugTools();
#endif

    window = window_factory->create(1024, 768, "3dfx");
    graphics_engine = graphics_engine_factory->create(window);
    run(os_msg_handler);
}
catch (const exception& ex)
{
    DEBUG_LOG << ex.what();
    throw;
}
