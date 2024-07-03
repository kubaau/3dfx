#pragma once

#include "WinTypedefs.hpp"
#include "WindowFactory.hpp"

class WinApiWindowFactory : public WindowFactory
{
public:
    WinApiWindowFactory(Instance);

    WindowPtr create(Width, Height, Title) override;

private:
    Instance instance;
};
