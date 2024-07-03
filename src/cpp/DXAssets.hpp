#pragma once

#include <tools/TaskScheduler.hpp>
#include "DXTypedefs.hpp"
#include "DXWavesCpu.hpp"

struct DXCore;
struct DXSettings;

struct DXAssets
{
    DXAssets(DXCore&, DXSettings&);

    Meshes meshes;
    Materials materials;
    Textures textures;

    void update();
    void activate(const Renderables&);

private:
    struct TexInfo
    {
        string filename, extension, name;
        unsigned array_count;
    };

    void initMeshes();
    void buildGeneratedMesh(const struct GeneratedMesh&, ID);
    void buildBox();
    void buildSphere();
    void buildGeosphere();
    void buildCylinder();
    void buildGrid();
    void buildLand();
    void buildSkullRoom();
    void buildWavesCpu();
    void loadFileMeshes();
    void initMaterials();
    void initTextures();
    void initTexArray(const TexInfo&);
    void initSingleTex(const TexInfo&);
    void initMultipleTex(const TexInfo&);
    SRVPtr loadDDSTexture(FilePath);
    SRVPtr loadBMPTexture(FilePath);
    SRVPtr create2DTextureArray(const Filenames&);

    DXCore& core;
    DXSettings& settings;

    TaskScheduler ts;
    WavesCpu waves_cpu;
};
