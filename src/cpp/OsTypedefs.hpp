#pragma once

#include <memory>
#include <string>

using Width = unsigned;
using Height = unsigned;
using Title = string;
using WindowHandle = size_t;
using WindowPtr = shared_ptr<class Window>;
using WindowFactoryPtr = shared_ptr<class WindowFactory>;
