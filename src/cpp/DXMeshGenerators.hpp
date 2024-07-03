#pragma once

#include <vector>
#include "DXMathTypedefs.hpp"

struct GeneratedMesh
{
    struct Vertex
    {
        Vertex() = default;
        Vertex(const float3& pos, const float3& normal, const float3& tangent_u, const float2& tex)
            : pos(pos), normal(normal), tangent_u(tangent_u), tex(tex)
        {
        }
        Vertex(
            float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz, float u, float v)
            : pos(px, py, pz), normal(nx, ny, nz), tangent_u(tx, ty, tz), tex(u, v)
        {
        }

        float3 pos;
        float3 normal;
        float3 tangent_u;
        float2 tex;
    };

    using Vertices = vector<Vertex>;
    using Indices = vector<unsigned>;

    Vertices vertices;
    Indices indices;
};

using WidthF = float;
using HeightF = WidthF;
using Depth = WidthF;
using Radius = WidthF;
using TopRadius = Radius;
using BottomRadius = Radius;
using DXCount = unsigned;
using StackCount = DXCount;
using SliceCount = DXCount;
using RowCount = DXCount;
using ColumnCount = DXCount;
using SubdivisionCount = DXCount;

GeneratedMesh generateBox(WidthF, HeightF, Depth);
GeneratedMesh generateSphere(Radius, StackCount, SliceCount);
GeneratedMesh generateGeosphere(Radius, SubdivisionCount);
GeneratedMesh generateCylinder(TopRadius, BottomRadius, HeightF, StackCount, SliceCount);
GeneratedMesh generateGrid(WidthF, Depth, RowCount, ColumnCount);
GeneratedMesh generateLand(WidthF, Depth, RowCount, ColumnCount);
