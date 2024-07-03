#pragma once

#include "OsTypedefs.hpp"
#include "WinTypedefs.hpp"
#include "Window.hpp"

class WinApiWindow : public Window
{
public:
    WinApiWindow(Instance, Width, Height, Title);

    void capture() override;
    void releaseCapture() override;

    void appendToTitle(string) override;

    Width getWidth() const override;
    Height getHeight() const override;
    WindowHandle getHandle() const override;

private:
    WinApiWindowHandle handle;
    Width width;
    Height height;
    Title title;
};
