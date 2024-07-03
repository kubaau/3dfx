#include "WinMainWndProc.hpp"
#include <windowsx.h>
#include "App.hpp" // :(
#include "WindowEvents.hpp"

namespace
{
    template <class Event, typename... Args>
    void addWindowEvent(Args&&... args)
    {
        window_events.push(make_unique<Event>(forward<Args>(args)...));
    }
} // namespace

LRESULT CALLBACK MainWndProc(WinApiWindowHandle window, MsgId msg, WPARAM w_param, LPARAM l_param)
{
    constexpr auto OK = 0;
    const Point p{GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)};

    switch (msg)
    {
        case WM_ACTIVATE:
            LOWORD(w_param) == WA_INACTIVE ? addWindowEvent<DeactivatedWindowEvent>() :
                                             addWindowEvent<ActivatedWindowEvent>();
            return OK;

        case WM_SIZE:
            switch (w_param)
            {
                case SIZE_MINIMIZED: addWindowEvent<MinimizedWindowEvent>(); break;
                case SIZE_MAXIMIZED: addWindowEvent<MaximizedWindowEvent>(); break;
                case SIZE_RESTORED: addWindowEvent<RestoredWindowEvent>(); break;
            }
            return OK;

        case WM_ENTERSIZEMOVE: addWindowEvent<ResizingStartedWindowEvent>(); return OK;
        case WM_EXITSIZEMOVE: addWindowEvent<ResizingStoppedWindowEvent>(); return OK;

        case WM_GETMINMAXINFO:
        {
            // this is needed because grabbing resizing bars causes Window to generate a blocking flow of events and we
            // essentially do not return to the app as long as the user is holding the bars, therefore using
            // WindowEvents is useless here
            app.pause();

            // prevent the window from becoming too small
            constexpr auto min_width = 200;
            constexpr auto min_height = min_width;
            reinterpret_cast<MINMAXINFO*>(l_param)->ptMinTrackSize = {min_width, min_height};
            return OK;
        }

        case WM_LBUTTONDOWN: addWindowEvent<LmbDownEvent>(p); return OK;
        case WM_MBUTTONDOWN: addWindowEvent<MmbDownEvent>(p); return OK;
        case WM_RBUTTONDOWN: addWindowEvent<RmbDownEvent>(p); return OK;
        case WM_LBUTTONUP: addWindowEvent<LmbUpEvent>(p); return OK;
        case WM_MBUTTONUP: addWindowEvent<MmbUpEvent>(p); return OK;
        case WM_RBUTTONUP: addWindowEvent<RmbUpEvent>(p); return OK;
        case WM_MOUSEMOVE: addWindowEvent<MouseMovedEvent>(p); return OK;

        case WM_MENUCHAR:
            // the WM_MENUCHAR message is sent when a menu is active and the user presses a key that does not correspond
            // to any mnemonic or accelerator key
            return MAKELRESULT(0, MNC_CLOSE); // don't beep when we alt-enter

        case WM_DESTROY: PostQuitMessage(OK); return OK;
    }

    return DefWindowProc(window, msg, w_param, l_param);
}
