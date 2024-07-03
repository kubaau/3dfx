#pragma once

#include <ext/dxerr.h>
#include <iostream>

#include "DXMathTypedefs.hpp"

#ifdef _DEBUG
#define HR(x)                            \
    {                                    \
        const auto hr = (x);             \
        if (FAILED(hr))                  \
            DXTRACE_ERR_MSGBOX(L#x, hr); \
    }
#else
#define HR(x) (x)
#endif

#define DECLARE_OSTREAM_OPERATOR(T) ostream& operator<<(ostream&, const T&)
#define DECLARE_OSTREAM_OPERATOR_STRUCT(T) ostream& operator<<(ostream&, const struct T&)
DECLARE_OSTREAM_OPERATOR(float2);
DECLARE_OSTREAM_OPERATOR(float3);
DECLARE_OSTREAM_OPERATOR(float4);
DECLARE_OSTREAM_OPERATOR(float4x4);
DECLARE_OSTREAM_OPERATOR(Matrix);
DECLARE_OSTREAM_OPERATOR_STRUCT(Material);
DECLARE_OSTREAM_OPERATOR_STRUCT(CBPerFrame);
DECLARE_OSTREAM_OPERATOR_STRUCT(CBPerObject);
DECLARE_OSTREAM_OPERATOR_STRUCT(DirectionalLight);
DECLARE_OSTREAM_OPERATOR_STRUCT(PointLight);
DECLARE_OSTREAM_OPERATOR_STRUCT(SpotLight);
#undef DECLARE_OSTREAM_OPERATOR
#undef DECLARE_OSTREAM_OPERATOR_STRUCT
