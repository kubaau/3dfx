#include "DXEngineFactory.hpp"
#include "DXEngine.hpp"

GraphicsEnginePtr DXEngineFactory::create(WindowPtr window)
{
    return make_unique<DXEngine>(move(window));
}
