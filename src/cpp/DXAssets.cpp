#include "DXAssets.hpp"

#include <ext/DDSTextureLoader.h>
#include <ext/WICTextureLoader.h>
#include <fstream>
#include <tools/Contains.hpp>
#include <tools/Random.hpp>
#include <tools/Repeat.hpp>

#include "CheckResult.hpp"
#include "DXCore.hpp"
#include "DXDebugUtils.hpp"
#include "DXMaterial.hpp"
#include "DXMesh.hpp"
#include "DXMeshGenerators.hpp"
#include "DXRenderable.hpp"
#include "DXSettings.hpp"
#include "DXVertex.hpp"
#include "ReadFile.hpp"

constexpr RowCount waves_cpu_row_count = 160;
constexpr ColumnCount waves_cpu_column_count = waves_cpu_row_count;
constexpr TimeStep waves_cpu_time_step = 0.03f;

DXAssets::DXAssets(DXCore& core, DXSettings& settings)
    : core(core),
      settings(settings),
      waves_cpu(*core.context,
                waves_cpu_row_count,
                waves_cpu_column_count,
                SpatialStep{1},
                waves_cpu_time_step,
                Speed{5},
                Damping{0.3f})
{
    initMeshes();
    initMaterials();
    initTextures();
}

void DXAssets::update()
{
    ts.launch();
}

void DXAssets::activate(const Renderables& renderables)
{
    if (contains(renderables, "waves_cpu"))
        waves_cpu.activate();
}

void DXAssets::initMeshes()
{
    buildBox();
    buildSphere();
    buildGeosphere();
    buildCylinder();
    buildGrid();
    buildLand();
    buildSkullRoom();
    buildWavesCpu();

    loadFileMeshes();
}

void DXAssets::initMaterials()
{
    auto file = openFile("../../assets/materials.txt");
    string line;
    getline(file, line); // skip first
    while (not file.eof())
    {
        Material m{};
        string id;
        file >> id;
        file >> m.ambient.x >> m.ambient.y >> m.ambient.z >> m.ambient.w;
        file >> m.diffuse.x >> m.diffuse.y >> m.diffuse.z >> m.diffuse.w;
        file >> m.specular.x >> m.specular.y >> m.specular.z >> m.specular.w;
        file >> m.reflect.x >> m.reflect.y >> m.reflect.z >> m.reflect.w;
        materials.emplace(id, m);
    }
}

void DXAssets::initTextures()
{
    auto file = openFile("../../assets/textures.txt");

    TexInfo tex;

    while (not file.eof())
    {
        file >> tex.filename >> tex.extension >> tex.name >> tex.array_count;

        if (contains(tex.name, "array")) // todo
            initTexArray(tex);
        else if (not tex.array_count)
            initSingleTex(tex);
        else
            initMultipleTex(tex);
    }
}

void DXAssets::initTexArray(const TexInfo& tex)
{
    Filenames filenames;
    repeat(tex.array_count) filenames.push_back(tex.filename + to_string(i) + "." + tex.extension);
    DEBUG_LOG << "Loading tex array " << tex.name << " - " << filenames;
    textures.emplace(tex.name, create2DTextureArray(filenames));
}

namespace
{
    const string tex_dir = "../../../assets/textures/";
}

void DXAssets::initSingleTex(const TexInfo& tex)
{
    const auto filename = tex.filename + "." + tex.extension;
    const auto path = tex_dir + filename;
    DEBUG_LOG << "Loading " << filename << " as " << tex.name;
    if (tex.extension == "dds")
        textures.emplace(tex.name, loadDDSTexture(path));
    else if (tex.extension == "bmp")
        textures.emplace(tex.name, loadBMPTexture(path));
}

void DXAssets::initMultipleTex(const TexInfo& tex)
{
    repeat(tex.array_count)
    {
        stringstream n;
        n.width(3);
        n.fill('0');
        n << i + 1;
        const auto filename = tex.filename + n.str() + "." + tex.extension;
        const auto path = tex_dir + filename;
        const auto key = tex.name + n.str();
        DEBUG_LOG << "Loading " << filename << " as " << key;
        if (tex.extension == "dds")
            textures.emplace(key, loadDDSTexture(path));
        else if (tex.extension == "bmp")
            textures.emplace(key, loadBMPTexture(path));
    }
}

#define LOAD_TEX(Type)                                                                                  \
    DXResourcePtr rsrc;                                                                                 \
    SRVPtr srv;                                                                                         \
    HR(Create##Type##TextureFromFile(                                                                   \
        core.device.Get(), core.context.Get(), path.c_str(), rsrc.GetAddressOf(), srv.GetAddressOf())); \
    return srv;

SRVPtr DXAssets::loadDDSTexture(FilePath path)
{
    LOAD_TEX(DDS);
}

SRVPtr DXAssets::loadBMPTexture(FilePath path)
{
    LOAD_TEX(WIC);
}

SRVPtr DXAssets::create2DTextureArray(const Filenames& filenames)
{
    // Load the texture elements individually from file. These textures won't be used by the GPU (0 bind flags), they
    // are just used to load the image data from file. We use the STAGING usage so the CPU can read the resource.

    const auto array_count = static_cast<unsigned>(filenames.size());

    vector<DXResourcePtr> rsrc(array_count);
    repeat(array_count)
    {
        HR(CreateDDSTextureFromFileEx(core.device.Get(),
                                      core.context.Get(),
                                      FilePath{tex_dir + filenames[i]}.c_str(),
                                      0,
                                      D3D11_USAGE_STAGING,
                                      0,
                                      D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
                                      0,
                                      DDS_LOADER_DEFAULT,
                                      rsrc[i].GetAddressOf(),
                                      nullptr));
    }

    D3D11_TEXTURE2D_DESC element_desc;
    Texture2DPtr tex2d;
    rsrc.front().As<Texture2D>(&tex2d);
    tex2d->GetDesc(&element_desc);

    D3D11_TEXTURE2D_DESC array_desc = element_desc;
    array_desc.ArraySize = array_count;
    array_desc.SampleDesc.Count = 1;
    array_desc.SampleDesc.Quality = 0;
    array_desc.Usage = D3D11_USAGE_DEFAULT;
    array_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    array_desc.CPUAccessFlags = 0;
    array_desc.MiscFlags = 0;

    Texture2DPtr tex_array;
    HR(core.device->CreateTexture2D(&array_desc, 0, &tex_array));

    repeat(array_count) repeat_j(element_desc.MipLevels)
    {
        D3D11_MAPPED_SUBRESOURCE mapped_tex_2d{};
        HR(core.context->Map(rsrc[i].Get(), j, D3D11_MAP_READ, 0, &mapped_tex_2d));

        core.context->UpdateSubresource(tex_array.Get(),
                                        D3D11CalcSubresource(j, i, element_desc.MipLevels),
                                        0,
                                        mapped_tex_2d.pData,
                                        mapped_tex_2d.RowPitch,
                                        mapped_tex_2d.DepthPitch);

        core.context->Unmap(rsrc[i].Get(), j);
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC view_desc{};
    view_desc.Format = array_desc.Format;
    view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    view_desc.Texture2DArray.MipLevels = array_desc.MipLevels;
    view_desc.Texture2DArray.ArraySize = array_count;

    SRVPtr tex;
    HR(core.device->CreateShaderResourceView(tex_array.Get(), &view_desc, &tex));
    return tex;
}

void DXAssets::buildGeneratedMesh(const GeneratedMesh& mesh, ID id)
{
    using Vertex = VertexPNT;

    const auto v_count = mesh.vertices.size();
    const auto i_count = mesh.indices.size();

    vector<Vertex> vs;
    vs.reserve(v_count);
    for (const auto& v : mesh.vertices)
        vs.emplace_back(v.pos, v.normal, v.tex);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_IMMUTABLE;

    D3D11_SUBRESOURCE_DATA init_data{};

    bd.ByteWidth = static_cast<unsigned>(sizeof(Vertex) * v_count);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    init_data.pSysMem = vs.data();
    D3DBufferPtr vb;
    HR(core.device->CreateBuffer(&bd, &init_data, &vb));

    bd.ByteWidth = static_cast<unsigned>(sizeof(unsigned) * i_count);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    init_data.pSysMem = mesh.indices.data();
    D3DBufferPtr ib;
    HR(core.device->CreateBuffer(&bd, &init_data, &ib));

    meshes.emplace(id,
                   Mesh{*core.context,
                        *settings.input_layouts.at("basic"),
                        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                        vb,
                        static_cast<Mesh::VertexCount>(v_count),
                        sizeof(Vertex),
                        ib,
                        static_cast<Mesh::IndexCount>(i_count)});
}

void DXAssets::buildBox()
{
    buildGeneratedMesh(generateBox(WidthF{1}, HeightF{1}, Depth{1}), "box");
}

void DXAssets::buildSphere()
{
    buildGeneratedMesh(generateSphere(Radius{1}, StackCount{20}, SliceCount{20}), "sphere");
}

void DXAssets::buildGeosphere()
{
    buildGeneratedMesh(generateGeosphere(Radius{1}, SubdivisionCount{3}), "geosphere");
}

void DXAssets::buildCylinder()
{
    buildGeneratedMesh(generateCylinder(TopRadius{0.6f}, BottomRadius{1}, HeightF{6}, StackCount{20}, SliceCount{20}),
                       "cylinder");
}

void DXAssets::buildGrid()
{
    buildGeneratedMesh(generateGrid(WidthF{1}, Depth{1}, RowCount{20}, ColumnCount{20}), "grid");
}

void DXAssets::buildLand()
{
    buildGeneratedMesh(generateLand(WidthF{160}, Depth{160}, RowCount{50}, ColumnCount{50}), "land");
}

void DXAssets::buildSkullRoom()
{
    using Vertex = VertexPNT;

    vector<Vertex> vs(30);

    // floor - tile texture coordinates
    vs[0] = {-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f};
    vs[1] = {-3.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    vs[2] = {7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f};

    vs[3] = {-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f};
    vs[4] = {7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f};
    vs[5] = {7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f};

    // wall - tile texture coordinates, leave a gap in the middle for the mirror
    vs[6] = {-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f};
    vs[7] = {-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
    vs[8] = {-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f};

    vs[9] = {-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f};
    vs[10] = {-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f};
    vs[11] = {-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f};

    vs[12] = {2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f};
    vs[13] = {2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
    vs[14] = {7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f};

    vs[15] = {2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f};
    vs[16] = {7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f};
    vs[17] = {7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f};

    vs[18] = {-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f};
    vs[19] = {-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
    vs[20] = {7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f};

    vs[21] = {-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f};
    vs[22] = {7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f};
    vs[23] = {7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f};

    // mirror
    vs[24] = {-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f};
    vs[25] = {-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
    vs[26] = {2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f};

    vs[27] = {-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f};
    vs[28] = {2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f};
    vs[29] = {2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f};

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init_data{};

    D3DBufferPtr room_vb;
    const auto v_count_room = static_cast<unsigned>(vs.size());
    bd.ByteWidth = sizeof(Vertex) * v_count_room;
    init_data.pSysMem = vs.data();
    HR(core.device->CreateBuffer(&bd, &init_data, &room_vb));

    meshes.emplace("room",
                   Mesh{*core.context,
                        *settings.input_layouts.at("basic"),
                        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                        room_vb,
                        static_cast<Mesh::VertexCount>(v_count_room),
                        sizeof(Vertex)});

    D3DBufferPtr floor_vb;
    constexpr auto v_count_floor = 6;
    bd.ByteWidth = sizeof(Vertex) * v_count_floor;
    init_data.pSysMem = vs.data();
    HR(core.device->CreateBuffer(&bd, &init_data, &floor_vb));

    meshes.emplace("floor",
                   Mesh{*core.context,
                        *settings.input_layouts.at("basic"),
                        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                        floor_vb,
                        static_cast<Mesh::VertexCount>(v_count_floor),
                        sizeof(Vertex)});

    D3DBufferPtr wall_vb;
    constexpr auto v_count_wall = 18;
    bd.ByteWidth = sizeof(Vertex) * v_count_wall;
    init_data.pSysMem = vs.data() + v_count_floor;
    HR(core.device->CreateBuffer(&bd, &init_data, &wall_vb));

    meshes.emplace("wall",
                   Mesh{*core.context,
                        *settings.input_layouts.at("basic"),
                        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                        wall_vb,
                        static_cast<Mesh::VertexCount>(v_count_wall),
                        sizeof(Vertex)});

    D3DBufferPtr mirror_vb;
    constexpr auto v_count_mirror = 6;
    bd.ByteWidth = sizeof(Vertex) * v_count_mirror;
    init_data.pSysMem = vs.data() + v_count_floor + v_count_wall;
    HR(core.device->CreateBuffer(&bd, &init_data, &mirror_vb));

    meshes.emplace("mirror",
                   Mesh{*core.context,
                        *settings.input_layouts.at("basic"),
                        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                        mirror_vb,
                        static_cast<Mesh::VertexCount>(v_count_mirror),
                        sizeof(Vertex)});
}

void DXAssets::buildWavesCpu()
{
    using Vertex = VertexPNT;

    const auto v_count = waves_cpu.getVertexCount();

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * v_count;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3DBufferPtr vb;
    HR(core.device->CreateBuffer(&bd, 0, &vb));

    vector<unsigned> is(waves_cpu.getTriangleCount() * 3);

    constexpr auto m = waves_cpu_row_count;
    constexpr auto n = waves_cpu_column_count;
    auto k = 0;
    for (auto i = 0 & m; i < m - 1; ++i)
    {
        for (auto j = 0 & n; j < n - 1; ++j)
        {
            is[k] = i * n + j;
            is[k + 1] = i * n + j + 1;
            is[k + 2] = (i + 1) * n + j;

            is[k + 3] = (i + 1) * n + j;
            is[k + 4] = i * n + j + 1;
            is[k + 5] = (i + 1) * n + j + 1;

            k += 6; // next quad
        }
    }

    const auto i_count = static_cast<unsigned>(is.size());

    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(unsigned) * i_count;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA init_data{};
    init_data.pSysMem = is.data();
    D3DBufferPtr ib;
    HR(core.device->CreateBuffer(&bd, &init_data, &ib));

    meshes.emplace("waves_cpu",
                   Mesh{*core.context,
                        *settings.input_layouts.at("basic"),
                        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                        vb,
                        v_count,
                        sizeof(Vertex),
                        ib,
                        i_count});

    waves_cpu.setVb(vb.Get());

    ts.schedule(
        [this] {
            const auto x = 5 + randomInt(0u, static_cast<unsigned>(waves_cpu.getWidth()) - 10);
            const auto y = 5 + randomInt(0u, static_cast<unsigned>(waves_cpu.getDepth()) - 10);
            const auto r = randomFloat(0.25f, 1.0f);
            waves_cpu.disturb(x, y, r);
        },
        20ms,
        unlimited_repetitions);

    ts.schedule([this] { waves_cpu.update(); },
                milliseconds{static_cast<milliseconds::rep>(waves_cpu_time_step * milli::den)},
                unlimited_repetitions);
}

static auto removeExtension(string filename)
{
    const auto lastdot = filename.find_last_of(".");
    return lastdot == string::npos ? filename : filename.substr(0, lastdot);
}

void DXAssets::loadFileMeshes()
{
    const string filename = "../../assets/models.txt";
    ifstream file{filename};
    check(file, "Cannot open " + filename);
    string mesh_name;
    while (getline(file, mesh_name))
        meshes.emplace(removeExtension(mesh_name),
                       Mesh{*core.context,
                            *settings.input_layouts.at("basic"),
                            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                            *core.device,
                            mesh_name});
}
