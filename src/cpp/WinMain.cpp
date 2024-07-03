#include <tools/Pause.hpp>
#include <tools/WinConsole.hpp>
#include "DXEngine.hpp"
#include "DXEngineFactory.hpp"
#include "Main.hpp"
#include "WinApiWindowFactory.hpp"
#include "WinMsgHandler.hpp"

int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) try
{
    WinMsgHandler msg_handler;
    runMain(
        make_unique<WinApiWindowFactory>(instance), make_unique<DXEngineFactory>(), msg_handler, enableDebugConsole);
}
catch (const exception&)
{
    winIoPause();
    exit(EXIT_FAILURE);
}
