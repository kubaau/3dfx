#pragma once

#include "DXMathTypedefs.hpp"
#include "DXMesh.hpp"
#include "DXTypedefs.hpp"

using DXCount = unsigned;
using RowCount = DXCount;
using ColumnCount = DXCount;
using SpatialStep = float;
using TimeStep = float;
using Speed = float;
using Damping = float;
using CoordX = unsigned;
using CoordY = unsigned;
using Magnitude = float;

class WavesCpu
{
    using Width = float;
    using Depth = float;

public:
    WavesCpu(Context&, RowCount, ColumnCount, SpatialStep, TimeStep, Speed, Damping);

    void activate() { activated = true; }
    void disturb(CoordX, CoordY, Magnitude);
    void update();

    void setVb(D3DBuffer* b) { vb = b; }

    auto getVertexCount() const { return v_count; }
    auto getTriangleCount() const { return triangle_count; }
    auto getRowCount() const { return row_count; }
    auto getColumnCount() const { return column_count; }
    auto getWidth() const { return width; }
    auto getDepth() const { return depth; }

private:
    bool activated = false;

    Context& context;

    RowCount row_count;
    ColumnCount column_count;
    SpatialStep spatial_step;
    TimeStep time_step;

    Width width;
    Depth depth;
    DXCount v_count;
    DXCount triangle_count;
    float k1, k2, k3;

    D3DBuffer* vb{};

    vector<float3> prev;
    vector<float3> curr;
    vector<float3> normals;
    vector<float3> tangent_x;
};
