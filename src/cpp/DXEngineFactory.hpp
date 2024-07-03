#pragma once

#include "GraphicsEngineFactory.hpp"

class DXEngineFactory : public GraphicsEngineFactory
{
public:
    GraphicsEnginePtr create(WindowPtr) override;
};
