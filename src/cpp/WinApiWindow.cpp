#include "WinApiWindow.hpp"
#include "CheckResult.hpp"
#include "WinMainWndProc.hpp"

namespace
{
    void registerClass(Instance instance, Title title)
    {
        WNDCLASS wc;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = MainWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = instance;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
        wc.lpszMenuName = 0;
        wc.lpszClassName = title.c_str();

        check(RegisterClass(&wc), "Failed to register class");
    }

    auto createRect(Width width, Height height)
    {
        return RECT{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    }

    auto createWindow(Instance instance, Title title, Width width, Height height)
    {
        auto r = createRect(width, height);
        AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
        const auto t = title.c_str();
        return CreateWindow(t, t, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, instance, 0);
    }

    void show(WinApiWindowHandle w)
    {
        ShowWindow(w, SW_SHOW);
        UpdateWindow(w);
    }
} // namespace

WinApiWindow::WinApiWindow(Instance instance, Width width, Height height, Title title)
    : width(width), height(height), title(title)
{
    registerClass(instance, title);
    handle = createWindow(instance, title, width, height);
    check(handle, "Failed to register class");
    show(handle);
}

void WinApiWindow::capture()
{
    SetCapture(handle);
}

void WinApiWindow::releaseCapture()
{
    ReleaseCapture();
}

void WinApiWindow::appendToTitle(string s)
{
    SetWindowText(handle, (title + s).c_str());
}

Width WinApiWindow::getWidth() const
{
    return width;
}

Height WinApiWindow::getHeight() const
{
    return height;
}

WindowHandle WinApiWindow::getHandle() const
{
    return reinterpret_cast<WindowHandle>(handle);
}
