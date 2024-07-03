#pragma once

#include <functional>
#include "GraphicsTypedefs.hpp"
#include "OsTypedefs.hpp"

using OsEnableDebugToolsFunc = function<void()>;

class OsMsgHandler;
void runMain(WindowFactoryPtr, GraphicsEngineFactoryPtr, OsMsgHandler&, OsEnableDebugToolsFunc);
