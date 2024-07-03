#include "DXMesh.hpp"
#include <tools/Repeat.hpp>
#include <tools/ThreadSafeLogger.hpp>
#include <fstream>
#include "CheckResult.hpp"
#include "DXDebugUtils.hpp"
#include "DXVertex.hpp"

Mesh::Mesh(Context& context,
           InputLayout& il,
           Topology topology,
           D3DBufferPtr vb,
           VertexCount v_count,
           Stride stride,
           D3DBufferPtr ib,
           IndexCount i_count,
           DXGI_FORMAT i_format)
    : context(context),
      il(il),
      topology(topology),
      vb(vb),
      v_count(v_count),
      stride(stride),
      ib(ib),
      i_count(i_count),
      i_format(i_format)
{
}

Mesh::Mesh(Context& context, InputLayout& il, Topology topology, Device& device, Filename filename)
    : context(context), il(il), topology(topology)
{
    DEBUG_LOG << "Loading " << filename;

    using Vertex = VertexPNT;

    constexpr auto base_filename = "../../../assets/models/";
    const string file_path = base_filename + filename;
    ifstream file_in{file_path};
    check(file_in, "Model not found - " + file_path);

    stride = sizeof(Vertex);

    DXCount triangle_count = 0;
    string ignored;
    file_in >> ignored >> v_count;
    file_in >> ignored >> triangle_count;
    i_count = triangle_count * 3;

    vector<Vertex> vs(v_count);
    vector<unsigned> indices(i_count);

    file_in >> ignored >> ignored >> ignored >> ignored;

    repeat(v_count)
    {
        file_in >> vs[i].pos.x >> vs[i].pos.y >> vs[i].pos.z;
        file_in >> vs[i].normal.x >> vs[i].normal.y >> vs[i].normal.z;
    }

    file_in >> ignored;
    file_in >> ignored;
    file_in >> ignored;

    repeat(triangle_count)
    {
        const auto i_mul3 = i * 3;
        file_in >> indices[i_mul3 + 0] >> indices[i_mul3 + 1] >> indices[i_mul3 + 2];
    }

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_IMMUTABLE;

    D3D11_SUBRESOURCE_DATA init_data{};

    bd.ByteWidth = sizeof(Vertex) * v_count;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    init_data.pSysMem = vs.data();
    HR(device.CreateBuffer(&bd, &init_data, &vb));

    bd.ByteWidth = sizeof(unsigned) * i_count;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    init_data.pSysMem = indices.data();
    HR(device.CreateBuffer(&bd, &init_data, &ib));

    i_format = DXGI_FORMAT_R32_UINT;
}

void Mesh::configureInputAssembler(Offset offset)
{
    context.IASetInputLayout(&il);

    context.IASetPrimitiveTopology(topology);

    constexpr auto vb_start_slot = 0;
    constexpr auto vb_count = 1;
    const auto byte_offset = offset * static_cast<unsigned>(sizeof(VertexPNT));
    context.IASetVertexBuffers(vb_start_slot, vb_count, vb.GetAddressOf(), &stride, &byte_offset);

    if (ib)
    {
        constexpr auto i_offset = 0;
        context.IASetIndexBuffer(ib.Get(), i_format, i_offset);
    }
}

void Mesh::draw(VertexCount vertex_count)
{
    constexpr auto start_index = 0;
    constexpr auto base_vertex = 0;
    constexpr auto start_vertex = 0;
    if (ib)
        context.DrawIndexed(i_count, start_index, base_vertex);
    else
        context.Draw(vertex_count ? vertex_count : v_count, start_vertex);
}
