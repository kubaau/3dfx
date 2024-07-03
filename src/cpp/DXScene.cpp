#include "DXScene.hpp"
#include <tools/KeysValues.hpp>
#include <fstream>
#include "CheckResult.hpp"
#include "DXAssets.hpp"
#include "DXCore.hpp"
#include "DXDebugUtils.hpp"
#include "DXMesh.hpp"
#include "DXRenderable.hpp"
#include "DXSettings.hpp"
#include "ReadFile.hpp"
#include "Timer.hpp"
#include "Window.hpp"

DXScene::DXScene(DXCore& core, DXSettings& settings, DXAssets& assets)
    : core(core), settings(settings), assets(assets), background_color(dark_gray)
{
    initScene();
    initLights();
}

void DXScene::onResize()
{
    updateProjection();
}

void DXScene::draw()
{
    update();

    clearViews();
    core.context->OMSetRenderTargets(1, core.rtv.GetAddressOf(), core.dsv.Get());

    const auto vp = XMLoadFloat4x4(&view_projection);
    drawSolid(vp);
    drawReflectedSolid(vp);
    drawTransparent(vp);

    HR(core.swap_chain->Present(0, 0));
}

void DXScene::orbit(float delta_theta, float delta_phi)
{
    theta += delta_theta;
    phi = clamp(phi + delta_phi, 0.1f, pi - 0.1f);
}

void DXScene::zoom(float delta_radius)
{
    radius = clamp(radius + delta_radius, 3.0f, 200.0f);
}

static bool isPressed(int key)
{
    return GetAsyncKeyState(key) & 0x8000;
}

void DXScene::update()
{
    assets.update();

    // updateBackgroundColor();
    updateEye();
    updateLights();

    handleKeyboard(); // not here

    settings.cb_per_frame.apply(*core.context);
}

void DXScene::updateBackgroundColor()
{
    const auto color = 0.5f + sin(timer.totalF()) / 2;
    background_color = {color, color, color, 1.0f};
}

void DXScene::updateEye()
{
    // settings.cb_per_frame.data.eye_pos = float3{-7.0f, 5.0f, -20.0f};

    const auto x = radius * sinf(phi) * cosf(theta);
    const auto z = radius * sinf(phi) * sinf(theta);
    const auto y = radius * cosf(phi);
    settings.cb_per_frame.data.eye_pos = {x, y, z};

    updateView();
}

void DXScene::updateView()
{
    const auto& eye_pos = settings.cb_per_frame.data.eye_pos;
    const auto camera_position = XMVectorSet(eye_pos.x, eye_pos.y, eye_pos.z, 1.0f);
    const auto camera_target = XMVectorZero();
    const auto camera_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    const auto v = XMMatrixLookAtLH(camera_position, camera_target, camera_up);
    XMStoreFloat4x4(&view, v);
    updateViewProjection();
}

void DXScene::updateLights()
{
    if (not point_lights.empty())
    {
        auto& pl = point_lights.front();
        const auto t_total = timer.totalF();
        pl.pos = {20 * cosf(1.2f * t_total), 10, 20 * sinf(1.2f * t_total)};
    }

    if (not spot_lights.empty())
    {
        auto& sl = spot_lights.front();
        sl.pos = settings.cb_per_frame.data.eye_pos;
        XMStoreFloat3(&sl.direction, XMVector3Normalize(XMVectorZero() - XMLoadFloat3(&sl.pos)));
    }

    copy(cbegin(dir_lights), cend(dir_lights), begin(settings.cb_per_frame.data.dir_lights));
    settings.cb_per_frame.data.dir_lights_count = static_cast<unsigned>(dir_lights.size());
    copy(cbegin(point_lights), cend(point_lights), begin(settings.cb_per_frame.data.point_lights));
    settings.cb_per_frame.data.point_lights_count = static_cast<unsigned>(point_lights.size());
    copy(cbegin(spot_lights), cend(spot_lights), begin(settings.cb_per_frame.data.spot_lights));
    settings.cb_per_frame.data.spot_lights_count = static_cast<unsigned>(spot_lights.size());
}

void DXScene::handleKeyboard()
{
    if (isPressed('R'))
        rs_override = settings.render_states["wireframe"].Get();
    else if (isPressed('C'))
    {
        constexpr D3D11_RECT rect = {100, 100, 400, 400};
        core.context->RSSetScissorRects(1, &rect);
        rs_override = settings.render_states["scissor"].Get();
    }
    else
        rs_override = nullptr;
}

void DXScene::initScene()
{
    auto file = openFile("../../assets/scene.txt");
    string line;

    // skip comments
    streampos g;
    do
    {
        g = file.tellg();
        getline(file, line);
    } while (line.empty() or line.front() == '#');
    file.seekg(g);

    ID renderable, mesh, material, tex, vertex_shader, pixel_shader, rasterizer_state, ds_state, blend_state;
    unsigned mesh_offset, vertex_count;
    bool has_tex_transforms, reflectable, reflecting;
    Rotation r;
    Scale s;
    Translation t;

    while (not file.eof())
    {
        file >> renderable >> mesh >> mesh_offset >> vertex_count >> material >> tex >> has_tex_transforms;

        float4x4 diffuse_transform;
        if (has_tex_transforms)
        {
            vector<float> transform(16);
            for (auto& v : transform)
                file >> v;
            diffuse_transform = float4x4{transform.data()};
        }
        else
        {
            XMStoreFloat4x4(&diffuse_transform, XMMatrixIdentity());
        }

        file >> vertex_shader >> pixel_shader >> rasterizer_state >> ds_state >> blend_state >> r.x >> r.y >> r.z >>
            s.x >> s.y >> s.z >> t.x >> t.y >> t.z >> reflectable >> reflecting;

        DEBUG_LOG_VAR(renderable)
        DEBUG_VAR(renderable)
        DEBUG_VAR(mesh)
        DEBUG_VAR(mesh_offset)
        DEBUG_VAR(vertex_count)
        DEBUG_VAR(material)
        DEBUG_VAR(tex)
        DEBUG_VAR(has_tex_transforms)
        DEBUG_VAR(diffuse_transform)
        DEBUG_VAR(vertex_shader)
        DEBUG_VAR(pixel_shader)
        DEBUG_VAR(rasterizer_state)
        DEBUG_VAR(ds_state)
        DEBUG_VAR(blend_state) DEBUG_VAR(r) DEBUG_VAR(s) DEBUG_VAR(t) DEBUG_VAR(reflectable) DEBUG_VAR(reflecting);

        r.x *= pi;
        r.y *= pi;
        r.z *= pi;

        try
        {
            renderables.emplace(renderable,
                                Renderable{*core.context,
                                           settings.cb_per_object,
                                           assets.meshes.at(mesh),
                                           mesh_offset,
                                           vertex_count,
                                           assets.textures.at(tex),
                                           assets.materials.at(material),
                                           *settings.render_states.at(rasterizer_state),
                                           *settings.ds_states.at(ds_state),
                                           *settings.blend_states.at(blend_state),
                                           *settings.vertex_shaders.at(vertex_shader),
                                           *settings.pixel_shaders.at(pixel_shader),
                                           move(r),
                                           move(s),
                                           move(t),
                                           move(diffuse_transform),
                                           reflectable,
                                           reflecting});
        }
        catch (const out_of_range&)
        {
#define PRINT_KEYS(map) DEBUG_LOG << #map ": " << keys(map)
            DEBUG_LOG << "Wrong renderable configuration, dumping all available names";
            PRINT_KEYS(assets.meshes);
            PRINT_KEYS(assets.materials);
            PRINT_KEYS(assets.textures);
            PRINT_KEYS(settings.render_states);
            PRINT_KEYS(settings.ds_states);
            PRINT_KEYS(settings.blend_states);
            PRINT_KEYS(settings.vertex_shaders);
            PRINT_KEYS(settings.pixel_shaders);
            throw;
        }
    }

    assets.activate(renderables);
}

void DXScene::initLights()
{
    auto file = openFile("../../assets/lights.txt");
    string line;
    getline(file, line); // skip first
    while (not file.eof())
    {
        file >> line;
        if (line == "dir")
        {
            DirectionalLight l;
            file >> l.ambient.x >> l.ambient.y >> l.ambient.z >> l.ambient.w;
            file >> l.diffuse.x >> l.diffuse.y >> l.diffuse.z >> l.diffuse.w;
            file >> l.specular.x >> l.specular.y >> l.specular.z >> l.specular.w;
            file >> l.direction.x >> l.direction.y >> l.direction.z;
            dir_lights.push_back(l);
        }
        else if (line == "point")
        {
            PointLight l;
            file >> l.ambient.x >> l.ambient.y >> l.ambient.z >> l.ambient.w;
            file >> l.diffuse.x >> l.diffuse.y >> l.diffuse.z >> l.diffuse.w;
            file >> l.specular.x >> l.specular.y >> l.specular.z >> l.specular.w;
            file >> l.pos.x >> l.pos.y >> l.pos.z;
            file >> l.range;
            file >> l.attenuation.x >> l.attenuation.y >> l.attenuation.z;
            point_lights.push_back(l);
        }
        else if (line == "spot")
        {
            SpotLight l;
            file >> l.ambient.x >> l.ambient.y >> l.ambient.z >> l.ambient.w;
            file >> l.diffuse.x >> l.diffuse.y >> l.diffuse.z >> l.diffuse.w;
            file >> l.specular.x >> l.specular.y >> l.specular.z >> l.specular.w;
            file >> l.direction.x >> l.direction.y >> l.direction.z;
            file >> l.pos.x >> l.pos.y >> l.pos.z;
            file >> l.range;
            file >> l.attenuation.x >> l.attenuation.y >> l.attenuation.z;
            file >> l.spot;
            spot_lights.push_back(l);
        }
    }
    DEBUG_LOG << "dir: " << dir_lights;
    DEBUG_LOG << "point: " << point_lights;
    DEBUG_LOG << "spot: " << spot_lights;
    check(not(dir_lights.size() > max_dir_lights), "Too many dir_lights");
    check(not(point_lights.size() > max_dir_lights), "Too many point_lights");
    check(not(spot_lights.size() > max_dir_lights), "Too many spot_lights");
}

void DXScene::updateProjection()
{
    XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(pi / 4, core.window->getAspectRatio(), 1.0f, 1000.0f));
    updateViewProjection();
}

void DXScene::updateViewProjection()
{
    XMStoreFloat4x4(&view_projection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));
}

void DXScene::clearViews()
{
    auto c = reinterpret_cast<const float*>(&background_color);
    core.context->ClearRenderTargetView(core.rtv.Get(), c);
    core.context->ClearRenderTargetView(core.offscreen_rtv.Get(), c);
    core.context->ClearDepthStencilView(core.dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DXScene::drawSolid(const Matrix& vp)
{
    for (const auto& r : renderables)
    {
        auto& rr = r.second;
        if (rr.isSolid())
            rr.render(vp, rs_override);
    }
}

void DXScene::drawReflectedSolid(const Matrix& vp)
{
    const auto xy_plane_reflection_matrix = XMMatrixReflect(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));

    auto reflected_lights = dir_lights;
    for (auto& light : reflected_lights)
        XMStoreFloat3(&light.direction,
                      XMVector3TransformNormal(XMLoadFloat3(&light.direction), xy_plane_reflection_matrix));
    copy(cbegin(reflected_lights), cend(reflected_lights), begin(settings.cb_per_frame.data.dir_lights));
    settings.cb_per_frame.apply(*core.context);

    for (const auto& r : renderables)
    {
        auto& rr = r.second;
        if (rr.isSolid() and rr.isReflectable())
            rr.render(vp,
                      rs_override == settings.render_states["wireframe"].Get() ?
                          settings.render_states["wireframe_cull_clockwise"].Get() :
                          settings.render_states["cull_clockwise"].Get(),
                      settings.ds_states["draw_reflection"].Get(),
                      &xy_plane_reflection_matrix);
    }

    copy(cbegin(dir_lights), cend(dir_lights), begin(settings.cb_per_frame.data.dir_lights));
    settings.cb_per_frame.apply(*core.context);
}

void DXScene::drawTransparent(const Matrix& vp)
{
    for (const auto& r : renderables)
    {
        auto& rr = r.second;
        if (rr.isTransparent())
            rr.render(vp, rs_override);
    }
}
