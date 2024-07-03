#pragma once

#include "GraphicsTypedefs.hpp"
#include "Interface.hpp"
#include "OsTypedefs.hpp"

class GraphicsEngineFactory : public Interface
{
public:
    virtual GraphicsEnginePtr create(WindowPtr) = 0;
};
