#pragma once

#include <tools/ThreadSafeLogger.hpp>
#include "DXDebugUtils.hpp"
#include "DXTypedefs.hpp"

template <class Data>
struct ConstantBuffer
{
    void init(Device&);
    void apply(Context&);

    Data data{};
    D3DBufferPtr buffer;
};

template <class Data>
void ConstantBuffer<Data>::init(Device& device)
{
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    // make constant buffer size a multiple of 16 bytes
    constexpr auto size = sizeof(Data);
    bd.ByteWidth = static_cast<unsigned>(size + (16 - size % 16));

    HR(device.CreateBuffer(&bd, nullptr, buffer.GetAddressOf()));
}

template <class Data>
void ConstantBuffer<Data>::apply(Context& context)
{
    //DEBUG_LOG << data;
    D3D11_MAPPED_SUBRESOURCE res;
    context.Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
    CopyMemory(res.pData, &data, sizeof(Data));
    context.Unmap(buffer.Get(), 0);
}
