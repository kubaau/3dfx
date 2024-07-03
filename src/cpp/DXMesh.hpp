#pragma once

#include "DXTypedefs.hpp"
#include "IoTypedefs.hpp"

class Mesh
{
    using DXCount = unsigned;
    using Stride = unsigned;
    using Offset = unsigned;

public:
    using VertexCount = DXCount;
    using IndexCount = DXCount;

    Mesh(Context&,
         InputLayout&,
         Topology,
         D3DBufferPtr,
         VertexCount,
         Stride,
         D3DBufferPtr = nullptr,
         IndexCount = 0,
         DXGI_FORMAT = DXGI_FORMAT_R32_UINT);

    Mesh(Context&, InputLayout&, Topology, Device&, Filename);

    void configureInputAssembler(Offset);
    void draw(VertexCount);

private:
    Context& context;
    InputLayout& il;
    Topology topology;

    D3DBufferPtr vb;
    VertexCount v_count;
    Stride stride;

    D3DBufferPtr ib;
    IndexCount i_count;
    DXGI_FORMAT i_format;
};
