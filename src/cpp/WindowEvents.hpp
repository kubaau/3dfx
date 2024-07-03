#pragma once

#include "Point.hpp"
#include "WindowEvent.hpp"

#define DEFINE_WINDOW_EVENT(Name)                \
    class Name##WindowEvent : public WindowEvent \
    {                                            \
    public:                                      \
        void handle() override;                  \
    };
DEFINE_WINDOW_EVENT(Activated)
DEFINE_WINDOW_EVENT(Deactivated)
DEFINE_WINDOW_EVENT(Minimized)
DEFINE_WINDOW_EVENT(Maximized)
DEFINE_WINDOW_EVENT(Restored)
DEFINE_WINDOW_EVENT(ResizingStarted)
DEFINE_WINDOW_EVENT(ResizingStopped)
#undef DEFINE_WINDOW_EVENT

#define DEFINE_MOUSE_EVENT(Name)           \
    class Name##Event : public WindowEvent \
    {                                      \
    public:                                \
        Name##Event(Point p) : p(p) {}     \
        void handle() override;            \
                                           \
    private:                               \
        Point p;                           \
    };
DEFINE_MOUSE_EVENT(LmbDown)
DEFINE_MOUSE_EVENT(MmbDown)
DEFINE_MOUSE_EVENT(RmbDown)
DEFINE_MOUSE_EVENT(LmbUp)
DEFINE_MOUSE_EVENT(MmbUp)
DEFINE_MOUSE_EVENT(RmbUp)
DEFINE_MOUSE_EVENT(MouseMoved)
#undef DEFINE_MOUSE_EVENT
