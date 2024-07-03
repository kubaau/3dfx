#include "WinMsgHandler.hpp"
#include "WinTypedefs.hpp"

namespace
{
    auto peek(WinMsg& msg) { return PeekMessage(&msg, 0, 0, 0, PM_REMOVE); }
    auto shouldQuit(const WinMsg& msg) { return msg.message == WM_QUIT; }

    WinMsgHandler::Result handle(const WinMsg& msg)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        return WinMsgHandler::HANDLED;
    }
} // namespace

WinMsgHandler::Result WinMsgHandler::handleOsMsgs()
{
    static WinMsg msg{};
    return peek(msg) ? handle(msg) : shouldQuit(msg) ? QUIT : NOT_HANDLED;
}
