#include "WinApiWindowFactory.hpp"
#include "WinApiWindow.hpp"

WinApiWindowFactory::WinApiWindowFactory(Instance instance) : instance(instance) {}

WindowPtr WinApiWindowFactory::create(Width width, Height height, Title title)
{
    return make_unique<WinApiWindow>(instance, width, height, title);
}
