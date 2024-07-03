#pragma once

#include "ConstantBuffer.hpp"
#include "DXTypedefs.hpp"

class Mesh;
struct Material;

class Renderable
{
    using CB = ConstantBuffer<struct CBPerObject>;

public:
    Renderable(Context&,
               CB&,
               Mesh&,
               unsigned mesh_offset,
               unsigned vertex_count,
               Texture&,
               const Material&,
               RState&,
               DSState&,
               BState&,
               VertexShader&,
               PixelShader&,
               Rotation,
               Scale,
               Translation,
               Transform diffuse_transform,
               bool reflectable,
               bool reflecting);

    void render(const Matrix& vp,
                RState* rs_override,
                DSState* dss_override = nullptr,
                const Matrix* reflection_matrix = nullptr) const;

    bool isSolid() const;
    bool isReflectable() const;
    bool isTransparent() const;

private:
    void updateWorld();

    Context& context;
    CB& cb;

    Mesh& mesh;
    unsigned mesh_offset;
    unsigned vertex_count;
    Texture& tex;
    const Material& mat;
    RState& rs;
    DSState& dss;
    BState& bs;
    VertexShader& vs;
    PixelShader& ps;

    Rotation r;
    Scale s;
    Translation t;
    Transform world;
    Transform diffuse_transform;

    bool reflectable;
    bool reflecting;
};
