#pragma once

#include "Interface.hpp"

class OsMsgHandler : public Interface
{
public:
    enum Result
    {
        NOT_HANDLED,
        HANDLED,
        QUIT
    };

    virtual Result handleOsMsgs() = 0;
};
