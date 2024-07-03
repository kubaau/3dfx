#pragma once

#include <string>
#include "OsTypedefs.hpp"

class Window
{
public:
    virtual ~Window() {}

    virtual void capture() = 0;
    virtual void releaseCapture() = 0;

    virtual void appendToTitle(string) = 0;

    virtual Width getWidth() const = 0;
    virtual Height getHeight() const = 0;
    virtual WindowHandle getHandle() const = 0;

    auto getAspectRatio() const { return static_cast<float>(getWidth()) / getHeight(); }
};

extern WindowPtr window;
