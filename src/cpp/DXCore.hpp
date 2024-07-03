#pragma once

#include "DXTypedefs.hpp"
#include "OsTypedefs.hpp"

struct DXCore
{
    DXCore(WindowPtr);

    void onResize();

    WindowPtr window;
    DevicePtr device;
    ContextPtr context;
    SwapChainPtr swap_chain;
    RTVPtr rtv;
    Texture2DPtr dsb;
    DSVPtr dsv;
    Viewport viewport{0, 0, 0, 0, 0, 1};
    SRVPtr offscreen_srv;
    UAVPtr offscreen_uav;
    RTVPtr offscreen_rtv;
    unsigned _4x_msaa_quality;

private:
    void initContext();
    void initSwapChain();

    void releaseOldComs();
    void updateSwapChain();
    void updateRtv();
    void updateDsb();
    void updateDsv();
    void updateViewport();
    void updateOffscreenViews();
};
