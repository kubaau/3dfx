#include "AppMainLoop.hpp"
#include <tools/TaskScheduler.hpp>
#include <tools/ThreadSafeLogger.hpp>
#include <sstream>
#include <thread>
#include "App.hpp"
#include "GraphicsEngine.hpp"
#include "OsMsgHandler.hpp"
#include "Timer.hpp"
#include "Window.hpp"
#include "WindowEvent.hpp"

namespace
{
    void updateTitle(unsigned& frame_count)
    {
        ostringstream oss;
        oss.precision(6);
        oss << " FPS: " << frame_count << ", frame time: ";
        if (frame_count)
            oss << milli::den / frame_count << "ms";
        else
            oss << ">1s";
        oss << ", total time: " << timer.total();
        window->appendToTitle(oss.str());

        frame_count = 0;
    }

    void handleWindowEvents()
    {
        while (not window_events.empty())
        {
            window_events.front()->handle();
            window_events.pop();
        }
    }

    void sleep(nanoseconds t) { this_thread::sleep_for(t); }
} // namespace

void run(OsMsgHandler& os_msg_handler)
{
    timer.tick();
    DEBUG_LOG << "Started running at time " << timer.total();

    unsigned frame_count = 0;

    TaskScheduler tasks;
    tasks.schedule([&frame_count] { updateTitle(frame_count); }, 1s, unlimited_repetitions);

    OsMsgHandler::Result result{};
    while (result != OsMsgHandler::QUIT)
    {
        timer.tick();
        tasks.launch();
        handleWindowEvents();

        result = os_msg_handler.handleOsMsgs();
        if (result == OsMsgHandler::HANDLED)
            continue;

        if (not app.paused)
        {
            ++frame_count;
            graphics_engine->run();
        }
        else
            sleep(100ms);
    }
}
