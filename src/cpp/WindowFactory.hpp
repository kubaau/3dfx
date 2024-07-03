#pragma once

#include "Interface.hpp"
#include "OsTypedefs.hpp"

class WindowFactory : public Interface
{
public:
    virtual WindowPtr create(Width, Height, Title) = 0;
};
