#include "DXRenderable.hpp"
#include "CBPerObject.hpp"
#include "DXMathUtils.hpp"
#include "DXMesh.hpp"

Renderable::Renderable(Context& context,
                       CB& cb,
                       Mesh& mesh,
                       unsigned mesh_offset,
                       unsigned vertex_count,
                       Texture& tex,
                       const Material& mat,
                       RState& rs,
                       DSState& dss,
                       BState& bs,
                       VertexShader& vs,
                       PixelShader& ps,
                       Rotation r,
                       Scale s,
                       Translation t,
                       Transform diffuse_transform,
                       bool reflectable,
                       bool reflecting)
    : context(context),
      cb(cb),
      mesh(mesh),
      mesh_offset(mesh_offset),
      vertex_count(vertex_count),
      tex(tex),
      mat(mat),
      rs(rs),
      dss(dss),
      bs(bs),
      vs(vs),
      ps(ps),
      r(move(r)),
      s(move(s)),
      t(move(t)),
      diffuse_transform(move(diffuse_transform)),
      reflectable(reflectable),
      reflecting(reflecting)
{
    updateWorld();
}

void Renderable::render(const Matrix& vp,
                        RState* rs_override,
                        DSState* dss_override,
                        const Matrix* reflection_matrix) const
{
    mesh.configureInputAssembler(mesh_offset);

    context.VSSetShader(&vs, nullptr, 0);
    context.PSSetShader(&ps, nullptr, 0);

    context.RSSetState(rs_override ? rs_override : &rs);

    constexpr auto no_stencil = 0;
    constexpr auto stencil_1 = 1;
    dss_override ? context.OMSetDepthStencilState(dss_override, stencil_1) :
                   context.OMSetDepthStencilState(&dss, no_stencil);

    constexpr float blend_factor[4] = {};
    constexpr auto blend_mask = 0xFFFFFFFF;
    context.OMSetBlendState(&bs, blend_factor, blend_mask);

    auto w = XMLoadFloat4x4(&world);
    if (reflection_matrix)
        w *= *reflection_matrix;
    const auto wit = inverseTranspose(w);
    const auto wvp = w * vp;

    if (tex)
    {
        context.PSSetShaderResources(0, 1, tex.GetAddressOf());

        // must pass transpose to convert to column-major format
        XMStoreFloat4x4(&cb.data.diffuse_transform, XMMatrixTranspose(XMLoadFloat4x4(&diffuse_transform)));
    }

    XMStoreFloat4x4(&cb.data.w, XMMatrixTranspose(w));
    XMStoreFloat4x4(&cb.data.wit, XMMatrixTranspose(wit));
    XMStoreFloat4x4(&cb.data.wvp, XMMatrixTranspose(wvp));
    cb.data.mat = mat;
    cb.apply(context);

    mesh.draw(vertex_count);
}

bool Renderable::isSolid() const
{
    return not isTransparent();
}

bool Renderable::isReflectable() const
{
    return reflectable;
}

bool Renderable::isTransparent() const
{
    return mat.diffuse.w < 1.0f;
}

void Renderable::updateWorld()
{
    const auto rm = XMMatrixRotationRollPitchYaw(r.x, r.y, r.z);
    const auto sm = XMMatrixScaling(s.x, s.y, s.z);
    const auto tm = XMMatrixTranslation(t.x, t.y, t.z);
    XMStoreFloat4x4(&world, rm * sm * tm);
}
