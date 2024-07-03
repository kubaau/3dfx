#include "DXWavesCpu.hpp"
#include <tools/Repeat.hpp>
#include <tools/ThreadSafeLogger.hpp>
#include "CheckResult.hpp"
#include "DXDebugUtils.hpp"
#include "DXVertex.hpp"

WavesCpu::WavesCpu(Context& context,
                   RowCount row_count,
                   ColumnCount column_count,
                   SpatialStep spatial_step,
                   TimeStep time_step,
                   Speed speed,
                   Damping damping)
    : context(context),
      row_count(row_count),
      column_count(column_count),
      spatial_step(spatial_step),
      time_step(time_step),
      width(column_count * spatial_step),
      depth(row_count * spatial_step),
      v_count(row_count * column_count),
      triangle_count((row_count - 1) * (column_count - 1) * 2)
{
    const auto c = damping * time_step;
    const auto d = c + 2;
    const auto e = pow(speed, 2) * pow(time_step, 2) / pow(spatial_step, 2);
    k1 = static_cast<float>((c - 2) / d);
    k2 = static_cast<float>((4 - 8 * e) / d);
    k3 = static_cast<float>(2 * e / d);

    prev.resize(v_count);
    curr.resize(v_count);
    normals.resize(v_count);
    tangent_x.resize(v_count);

    const auto width_div2 = (column_count - 1) * spatial_step / 2;
    const auto depth_div2 = (row_count - 1) * spatial_step / 2;
    repeat(row_count)
    {
        const auto z = depth_div2 - i * spatial_step;
        repeat_j(column_count)
        {
            const auto index = i * column_count + j;
            const auto x = -width_div2 + j * spatial_step;
            curr[index] = {x, 0, z};
            prev[index] = {x, 0, z};
            normals[index] = {0, 1, 0};
            tangent_x[index] = {1, 0, 0};
        }
    }
}

void WavesCpu::disturb(CoordX x, CoordY y, Magnitude magnitude)
{
    if (not activated)
        return;

    check(x > 1 && x < row_count - 2, "x coordinate out of bounds");
    check(y > 1 && y < column_count - 2, "y coordinate out of bounds");

    const auto magnitude_div2 = magnitude / 2;
    curr[x * column_count + y].y += magnitude;
    curr[x * column_count + y + 1].y += magnitude_div2;
    curr[x * column_count + y - 1].y += magnitude_div2;
    curr[(x + 1) * column_count + y].y += magnitude_div2;
    curr[(x - 1) * column_count + y].y += magnitude_div2;
}

void WavesCpu::update()
{
    if (not activated)
        return;

    for (auto i = 1u; i < row_count - 1; ++i)
    {
        for (auto j = 1u; j < column_count - 1; ++j)
        {
            prev[i * column_count + j].y = k1 * prev[i * column_count + j].y + k2 * curr[i * column_count + j].y +
                k3 *
                    (curr[(i + 1) * column_count + j].y + curr[(i - 1) * column_count + j].y +
                     curr[i * column_count + j + 1].y + curr[i * column_count + j - 1].y);
        }
    }

    swap(prev, curr);

    const auto spatial_step_mul2 = spatial_step * 2;
    for (auto i = 1u; i < row_count - 1; ++i)
    {
        for (auto j = 1u; j < column_count - 1; ++j)
        {
            const auto l = curr[i * column_count + j - 1].y;
            const auto r = curr[i * column_count + j + 1].y;
            const auto t = curr[(i - 1) * column_count + j].y;
            const auto b = curr[(i + 1) * column_count + j].y;
            normals[i * column_count + j].x = -r + l;
            normals[i * column_count + j].y = spatial_step_mul2;
            normals[i * column_count + j].z = b - t;

            XMStoreFloat3(&normals[i * column_count + j],
                          XMVector3Normalize(XMLoadFloat3(&normals[i * column_count + j])));

            tangent_x[i * column_count + j] = {spatial_step_mul2, r - l, 0.0f};
            XMStoreFloat3(&tangent_x[i * column_count + j],
                          XMVector3Normalize(XMLoadFloat3(&tangent_x[i * column_count + j])));
        }
    }

    D3D11_MAPPED_SUBRESOURCE mapped_data;
    HR(context.Map(vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_data));
    auto v = reinterpret_cast<VertexPNT*>(mapped_data.pData);
    repeat(v_count)
    {
        const auto& c = curr[i];
        v[i].pos = c;
        v[i].normal = normals[i];
        v[i].tex.x = 0.5f + c.x / width;
        v[i].tex.y = 0.5f - c.z / depth;
    }
    context.Unmap(vb, 0);
}
