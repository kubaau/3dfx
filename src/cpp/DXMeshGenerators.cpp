#include "DXMeshGenerators.hpp"
#include <tools/ContainerOperators.hpp>
#include <tools/Repeat.hpp>
#include "DXMathUtils.hpp"

using namespace RangeOperators;

using Vertex = GeneratedMesh::Vertex;

namespace
{
    auto generateBoxVertices(WidthF width, HeightF height, Depth depth)
    {
        GeneratedMesh::Vertices v(24);

        const auto width_half = width / 2;
        const auto height_half = height / 2;
        const auto depth_half = depth / 2;

        auto idx = 0;

        // front
        v[idx++] = {-width_half, -height_half, -depth_half, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        v[idx++] = {-width_half, +height_half, -depth_half, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        v[idx++] = {+width_half, +height_half, -depth_half, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};
        v[idx++] = {+width_half, -height_half, -depth_half, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f};

        // back
        v[idx++] = {-width_half, -height_half, +depth_half, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f};
        v[idx++] = {+width_half, -height_half, +depth_half, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        v[idx++] = {+width_half, +height_half, +depth_half, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        v[idx++] = {-width_half, +height_half, +depth_half, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

        // top
        v[idx++] = {-width_half, +height_half, -depth_half, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        v[idx++] = {-width_half, +height_half, +depth_half, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        v[idx++] = {+width_half, +height_half, +depth_half, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};
        v[idx++] = {+width_half, +height_half, -depth_half, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f};

        // bottom
        v[idx++] = {-width_half, -height_half, -depth_half, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f};
        v[idx++] = {+width_half, -height_half, -depth_half, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        v[idx++] = {+width_half, -height_half, +depth_half, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        v[idx++] = {-width_half, -height_half, +depth_half, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

        // left
        v[idx++] = {-width_half, -height_half, +depth_half, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f};
        v[idx++] = {-width_half, +height_half, +depth_half, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
        v[idx++] = {-width_half, +height_half, -depth_half, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f};
        v[idx++] = {-width_half, -height_half, -depth_half, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f};

        // right
        v[idx++] = {+width_half, -height_half, -depth_half, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f};
        v[idx++] = {+width_half, +height_half, -depth_half, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
        v[idx++] = {+width_half, +height_half, +depth_half, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f};
        v[idx++] = {+width_half, -height_half, +depth_half, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

        return v;
    }

    auto generateBoxIndices()
    {
        GeneratedMesh::Indices i(36);

        auto idx = 0;

        // front
        i[idx++] = 0;
        i[idx++] = 1;
        i[idx++] = 2;
        i[idx++] = 0;
        i[idx++] = 2;
        i[idx++] = 3;

        // back
        i[idx++] = 4;
        i[idx++] = 5;
        i[idx++] = 6;
        i[idx++] = 4;
        i[idx++] = 6;
        i[idx++] = 7;

        // top
        i[idx++] = 8;
        i[idx++] = 9;
        i[idx++] = 10;
        i[idx++] = 8;
        i[idx++] = 10;
        i[idx++] = 11;

        // bottom
        i[idx++] = 12;
        i[idx++] = 13;
        i[idx++] = 14;
        i[idx++] = 12;
        i[idx++] = 14;
        i[idx++] = 15;

        // left
        i[idx++] = 16;
        i[idx++] = 17;
        i[idx++] = 18;
        i[idx++] = 16;
        i[idx++] = 18;
        i[idx++] = 19;

        // right
        i[idx++] = 20;
        i[idx++] = 21;
        i[idx++] = 22;
        i[idx++] = 20;
        i[idx++] = 22;
        i[idx++] = 23;

        return i;
    }

    auto generateSphereVertices(Radius radius, StackCount stack_count, SliceCount slice_count)
    {
        GeneratedMesh::Vertices vs;

        // top
        vs += Vertex{0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f};

        const auto phi_step = pi / stack_count;
        const auto theta_step = XM_2PI / slice_count;

        for (auto i = 1u; i <= stack_count - 1; ++i)
        {
            const auto phi = i * phi_step;

            for (auto j = 0u; j <= slice_count; ++j)
            {
                const auto theta = j * theta_step;

                Vertex v;

                v.pos.x = radius * sinf(phi) * cosf(theta);
                v.pos.y = radius * cosf(phi);
                v.pos.z = radius * sinf(phi) * sinf(theta);

                v.tangent_u.x = -radius * sinf(phi) * sinf(theta);
                v.tangent_u.y = 0.0f;
                v.tangent_u.z = +radius * sinf(phi) * cosf(theta);

                XMStoreFloat3(&v.tangent_u, XMVector3Normalize(XMLoadFloat3(&v.tangent_u)));
                XMStoreFloat3(&v.normal, XMVector3Normalize(XMLoadFloat3(&v.pos)));

                v.tex.x = theta / XM_2PI;
                v.tex.y = phi / pi;

                vs += v;
            }
        }

        // bottom
        vs += Vertex{0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

        return vs;
    }

    auto generateSphereIndices(StackCount stack_count, SliceCount slice_count, DXCount v_count)
    {
        GeneratedMesh::Indices is;

        for (auto i = 1u; i <= slice_count; ++i)
        {
            is += 0;
            is += i + 1;
            is += i;
        }

        auto base_idx = 1;
        const auto ring_v_count = slice_count + 1;
        for (auto i = 0 & stack_count; i < stack_count - 2; ++i)
        {
            repeat_j(slice_count)
            {
                is += base_idx + i * ring_v_count + j;
                is += base_idx + i * ring_v_count + j + 1;
                is += base_idx + (i + 1) * ring_v_count + j;

                is += base_idx + (i + 1) * ring_v_count + j;
                is += base_idx + i * ring_v_count + j + 1;
                is += base_idx + (i + 1) * ring_v_count + j + 1;
            }
        }

        const auto south_pole_idx = v_count - 1;

        base_idx = south_pole_idx - ring_v_count;

        repeat(slice_count)
        {
            is += south_pole_idx;
            is += base_idx + i;
            is += base_idx + i + 1;
        }

        return is;
    }

    auto generateGeosphereVertices()
    {
        constexpr auto x = 0.525731f;
        constexpr auto z = 0.850651f;

        vector<float3> pos = {{-x, 0.0f, z},
                              {x, 0.0f, z},
                              {-x, 0.0f, -z},
                              {x, 0.0f, -z},
                              {0.0f, z, x},
                              {0.0f, z, -x},
                              {0.0f, -z, x},
                              {0.0f, -z, -x},
                              {z, x, 0.0f},
                              {-z, x, 0.0f},
                              {z, -x, 0.0f},
                              {-z, -x, 0.0f}};

        GeneratedMesh::Vertices vs(pos.size());
        repeat(pos.size()) vs[i].pos = pos[i];
        return vs;
    }

    GeneratedMesh::Indices generateGeosphereIndices()
    {
        return {1, 4,  0, 4,  9, 0, 4, 5,  9, 8, 5, 4,  1, 8, 4, 1,  10, 8, 10, 3, 8, 8, 3,  5, 3, 2, 5, 3,  7, 2,
                3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6, 1, 0, 10, 1,  6, 11, 0, 9, 2, 11, 9, 5, 2, 9, 11, 2, 7};
    }

    auto subdivide(GeneratedMesh in)
    {
        GeneratedMesh out;

        //       v1
        //       *
        //      / \
        //     /   \
        //  m0*-----*m1
        //   / \   / \
        //  /   \ /   \
        // *-----*-----*
        // v0    m2     v2

        const auto tri_count = static_cast<unsigned>(in.indices.size()) / 3;
        repeat(tri_count)
        {
            const auto& v0 = in.vertices[in.indices[i * 3 + 0]];
            const auto& v1 = in.vertices[in.indices[i * 3 + 1]];
            const auto& v2 = in.vertices[in.indices[i * 3 + 2]];

            // generate the midpoints
            Vertex m0, m1, m2;

            // for subdivision, we just care about the position component
            // we derive the other vertex components during projection
            m0.pos = {(v0.pos.x + v1.pos.x) / 2, (v0.pos.y + v1.pos.y) / 2, (v0.pos.z + v1.pos.z) / 2};
            m1.pos = {(v1.pos.x + v2.pos.x) / 2, (v1.pos.y + v2.pos.y) / 2, (v1.pos.z + v2.pos.z) / 2};
            m2.pos = {(v0.pos.x + v2.pos.x) / 2, (v0.pos.y + v2.pos.y) / 2, (v0.pos.z + v2.pos.z) / 2};

            // add new geometry
            out.vertices += v0; // 0
            out.vertices += v1; // 1
            out.vertices += v2; // 2
            out.vertices += m0; // 3
            out.vertices += m1; // 4
            out.vertices += m2; // 5

            out.indices += i * 6 + 0;
            out.indices += i * 6 + 3;
            out.indices += i * 6 + 5;

            out.indices += i * 6 + 3;
            out.indices += i * 6 + 4;
            out.indices += i * 6 + 5;

            out.indices += i * 6 + 5;
            out.indices += i * 6 + 4;
            out.indices += i * 6 + 2;

            out.indices += i * 6 + 3;
            out.indices += i * 6 + 1;
            out.indices += i * 6 + 4;
        }

        return out;
    }

    auto projectVerticesOntoSphere(GeneratedMesh& mesh, Radius radius)
    {
        // project vertices onto sphere and scale
        for (auto& v : mesh.vertices)
        {
            // project onto unit sphere
            const auto n = XMVector3Normalize(XMLoadFloat3(&v.pos));

            // project onto sphere
            XMStoreFloat3(&v.pos, radius * n);
            XMStoreFloat3(&v.normal, n);

            // derive texture coordinates from spherical coordinates
            const auto theta = angleFromXY(v.pos.x, v.pos.z);
            const auto phi = acosf(v.pos.y / radius);

            v.tex.x = theta / XM_2PI;
            v.tex.y = phi / XM_PI;

            // partial derivative of p with respect to theta
            v.tangent_u.x = -radius * sinf(phi) * sinf(theta);
            v.tangent_u.y = 0.0f;
            v.tangent_u.z = +radius * sinf(phi) * cosf(theta);

            XMStoreFloat3(&v.tangent_u, XMVector3Normalize(XMLoadFloat3(&v.tangent_u)));
        }
    }

    auto generateCylinderVertices(TopRadius top_radius,
                                  BottomRadius bottom_radius,
                                  HeightF height,
                                  StackCount stack_count,
                                  SliceCount slice_count)
    {
        GeneratedMesh::Vertices vs;

        const auto ring_count = stack_count + 1;
        const auto stack_height = height / stack_count;
        const auto radius_step = (top_radius - bottom_radius) / stack_count;
        const auto height_div2 = height / 2;
        const auto theta = XM_2PI / slice_count;

        repeat(ring_count)
        {
            const auto y = -height_div2 + i * stack_height;
            const auto r = bottom_radius + i * radius_step;

            for (auto j = 0 & slice_count; j <= slice_count; ++j)
            {
                Vertex vertex;

                const auto jt = j * theta;
                const auto c = cosf(jt);
                const auto s = sinf(jt);

                vertex.pos = {r * c, y, r * s};
                vertex.tex.x = static_cast<float>(j) / slice_count;
                vertex.tex.y = 1.0f - static_cast<float>(i) / stack_count;
                vertex.tangent_u = {-s, 0, c};

                const auto radius_diff = bottom_radius - top_radius;
                const float3 bitangent{radius_diff * c, -height, radius_diff * s};
                XMStoreFloat3(
                    &vertex.normal,
                    XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&vertex.tangent_u), XMLoadFloat3(&bitangent))));

                vs += vertex;
            }
        }

        return vs;
    }

    auto generateCylinderIndices(StackCount stack_count, SliceCount slice_count)
    {
        GeneratedMesh::Indices is;

        // duplicate the first and last vertex per ring since the texture coordinates are different
        const auto ring_v_count = slice_count + 1;

        repeat(stack_count)
        {
            repeat_j(slice_count)
            {
                is += i * ring_v_count + j;
                is += (i + 1) * ring_v_count + j;
                is += (i + 1) * ring_v_count + j + 1;

                is += i * ring_v_count + j;
                is += (i + 1) * ring_v_count + j + 1;
                is += i * ring_v_count + j + 1;
            }
        }

        return is;
    }

#define DEFINE_CAP_CONSTANTS                                           \
    const auto base_idx = static_cast<unsigned>(mesh.vertices.size()); \
    const auto theta = XM_2PI / slice_count;

#define DEFINE_CAP_LOOP_CONSTANTS(radius) \
    const auto it = i * theta;            \
    const auto x = radius * cosf(it);     \
    const auto z = radius * sinf(it);     \
    const auto u = x / height + 0.5f;     \
    const auto v = z / height + 0.5f;

    void generateCylinderTopCap(GeneratedMesh& mesh, TopRadius top_radius, HeightF height, SliceCount slice_count)
    {
        DEFINE_CAP_CONSTANTS;
        const auto y = height / 2;

        // duplicate cap ring vertices because the texture coordinates and normals differ
        for (auto i = 0 & slice_count; i <= slice_count; ++i)
        {
            DEFINE_CAP_LOOP_CONSTANTS(top_radius);
            mesh.vertices += Vertex{x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v};
        }
        // cap center
        mesh.vertices += Vertex{0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f};

        const auto center_idx = static_cast<unsigned>(mesh.vertices.size() - 1);
        repeat(slice_count)
        {
            mesh.indices += center_idx;
            mesh.indices += base_idx + i + 1;
            mesh.indices += base_idx + i;
        }
    }

    void generateCylinderBottomCap(GeneratedMesh& mesh,
                                   BottomRadius bottom_radius,
                                   HeightF height,
                                   SliceCount slice_count)
    {
        DEFINE_CAP_CONSTANTS;
        const auto y = -height / 2;

        for (auto i = 0 & slice_count; i <= slice_count; ++i)
        {
            DEFINE_CAP_LOOP_CONSTANTS(bottom_radius);
            mesh.vertices += Vertex{x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v};
        }
        mesh.vertices += Vertex{0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f};

        const auto center_idx = static_cast<unsigned>(mesh.vertices.size() - 1);
        repeat(slice_count)
        {
            mesh.indices += center_idx;
            mesh.indices += base_idx + i;
            mesh.indices += base_idx + i + 1;
        }
    }

    auto generateGridVertices(WidthF width, Depth depth, RowCount row_count, ColumnCount column_count)
    {
        GeneratedMesh::Vertices vs(row_count * column_count);

        const auto width_div2 = width / 2;
        const auto depth_div2 = depth / 2;

        const auto dx = width / (column_count - 1);
        const auto dz = depth / (row_count - 1);

        const auto du = 1.0f / (column_count - 1);
        const auto dv = 1.0f / (row_count - 1);

        repeat(row_count)
        {
            const auto z = depth_div2 - i * dz;
            repeat_j(column_count)
            {
                const auto x = -width_div2 + j * dx;

                vs[i * column_count + j].pos = {x, 0, z};
                vs[i * column_count + j].normal = {0, 1, 0};
                vs[i * column_count + j].tangent_u = {1, 0, 0};
                vs[i * column_count + j].tex.x = j * du;
                vs[i * column_count + j].tex.y = i * dv;
            }
        }

        return vs;
    }

    auto generateGridIndices(RowCount row_count, ColumnCount column_count)
    {
        const auto face_count = (row_count - 1) * (column_count - 1) * 2;
        constexpr auto indices_per_face = 3;
        GeneratedMesh::Indices is(face_count * indices_per_face);

        auto idx = 0;
        for (auto i = 0 & row_count; i < row_count - 1; ++i)
        {
            for (auto j = 0 & column_count; j < column_count - 1; ++j)
            {
                is[idx] = i * column_count + j;
                is[idx + 1] = i * column_count + j + 1;
                is[idx + 2] = (i + 1) * column_count + j;

                is[idx + 3] = (i + 1) * column_count + j;
                is[idx + 4] = i * column_count + j + 1;
                is[idx + 5] = (i + 1) * column_count + j + 1;

                idx += 6; // next quad
            }
        }

        return is;
    }

    auto hillHeight(float x, float z) { return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z)); }

    auto hillNormal(float x, float z)
    {
        // n = (-df/dx, 1, -df/dz)
        float3 n{-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
                 1.0f,
                 -0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z)};
        XMStoreFloat3(&n, XMVector3Normalize(XMLoadFloat3(&n)));
        return n;
    }
} // namespace

GeneratedMesh generateBox(WidthF width, HeightF height, Depth depth)
{
    return {generateBoxVertices(width, height, depth), generateBoxIndices()};
}

GeneratedMesh generateSphere(Radius radius, StackCount stack_count, SliceCount slice_count)
{
    GeneratedMesh mesh{generateSphereVertices(radius, stack_count, slice_count)};
    mesh.indices = generateSphereIndices(stack_count, slice_count, static_cast<unsigned>(mesh.vertices.size()));
    return mesh;
}

GeneratedMesh generateGeosphere(Radius radius, SubdivisionCount subdivision_count)
{
    // approximate a sphere by tessellating an icosahedron
    GeneratedMesh mesh{generateGeosphereVertices(), generateGeosphereIndices()};
    constexpr SubdivisionCount max_subdivision_count = 5;
    repeat(min(subdivision_count, max_subdivision_count)) mesh = subdivide(mesh);
    projectVerticesOntoSphere(mesh, radius);
    return mesh;
}

GeneratedMesh generateCylinder(
    TopRadius top_radius, BottomRadius bottom_radius, HeightF height, StackCount stack_count, SliceCount slice_count)
{
    GeneratedMesh mesh{generateCylinderVertices(top_radius, bottom_radius, height, stack_count, slice_count)};
    mesh.indices = generateCylinderIndices(stack_count, slice_count);
    generateCylinderTopCap(mesh, top_radius, height, slice_count);
    generateCylinderBottomCap(mesh, bottom_radius, height, slice_count);
    return mesh;
}

GeneratedMesh generateGrid(WidthF width, Depth depth, RowCount row_count, ColumnCount column_count)
{
    return {generateGridVertices(width, depth, row_count, column_count), generateGridIndices(row_count, column_count)};
}

GeneratedMesh generateLand(WidthF width, Depth depth, RowCount row_count, ColumnCount column_count)
{
    auto grid = generateGrid(width, depth, row_count, column_count);
    for (auto& v : grid.vertices)
    {
        v.pos.y = hillHeight(v.pos.x, v.pos.z);
        v.normal = hillNormal(v.pos.x, v.pos.z);
    }
    return grid;
}
