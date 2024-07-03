#pragma once

#include "CBPerFrame.hpp"
#include "CBPerObject.hpp"
#include "CBRarely.hpp"
#include "ConstantBuffer.hpp"
#include "DXTypedefs.hpp"

struct DXCore;

struct DXSettings
{
    DXSettings(DXCore&);

    void initShaders();
    void initRenderStates();
    void initRStates();
    void initDSStates();
    void initBStates();
    void initSStates();
    void initConstantBuffers();

    VertexShaders vertex_shaders;
    PixelShaders pixel_shaders;
    InputLayouts input_layouts;
    RStates render_states;
    DSStates ds_states;
    BStates blend_states;
    SStates sampler_states;
    ConstantBuffer<CBPerFrame> cb_per_frame;
    ConstantBuffer<CBPerObject> cb_per_object;
    ConstantBuffer<CBRarely> cb_rarely;

private:
    DXCore& core;
};
