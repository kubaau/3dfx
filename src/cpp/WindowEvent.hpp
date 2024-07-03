#pragma once

#include <memory>
#include <queue>
#include "Interface.hpp"

class WindowEvent : public Interface
{
public:
    virtual void handle() = 0;
};

using WindowEvents = queue<unique_ptr<WindowEvent>>;
extern WindowEvents window_events;
