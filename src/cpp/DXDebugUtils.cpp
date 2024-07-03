#include "DXDebugUtils.hpp"
#include <tools/OutputOperators.hpp>
#include "CBPerFrame.hpp"
#include "CBPerObject.hpp"
#include "DXLights.hpp"
#include "DXMaterial.hpp"

ostream& operator<<(ostream& os, const float2& f)
{
    return os << "{" << f.x << ", " << f.y << "}";
}

ostream& operator<<(ostream& os, const float3& f)
{
    return os << "{" << f.x << ", " << f.y << ", " << f.z << "}";
}

ostream& operator<<(ostream& os, const float4& f)
{
    return os << "{" << f.x << ", " << f.y << ", " << f.z << ", " << f.w << "}";
}

ostream& operator<<(ostream& os, const float4x4& m)
{
    os << "{" << m._11;
    os << ", " << m._12;
    os << ", " << m._13;
    os << ", " << m._14;
    os << "}";
    os << "{" << m._21;
    os << ", " << m._22;
    os << ", " << m._23;
    os << ", " << m._24;
    os << "}";
    os << "{" << m._31;
    os << ", " << m._32;
    os << ", " << m._33;
    os << ", " << m._34;
    os << "}";
    os << "{" << m._41;
    os << ", " << m._42;
    os << ", " << m._43;
    os << ", " << m._44;
    os << "}";
    return os;
}

ostream& operator<<(ostream& os, const Matrix& m)
{
    return os << reinterpret_cast<const float4x4&>(m);
}

ostream& operator<<(ostream& os, const Material& m)
{
    return os << m.ambient << m.diffuse << m.specular << m.reflect;
}

ostream& operator<<(ostream& os, const CBPerFrame& cb)
{
    return os << "{"
              << "eye_pos = " << cb.eye_pos << ", dir_lights = " << cb.dir_lights
              << ", point_lights = " << cb.point_lights << ", spot_lights = " << cb.spot_lights
              << ", dir_lights_count = " << cb.dir_lights_count << ", point_lights_count = " << cb.point_lights_count
              << ", spot_lights_count = " << cb.spot_lights_count << "}";
}

ostream& operator<<(ostream& os, const CBPerObject& cb)
{
    return os << "{"
              << "wvp = " << cb.wvp << ", mat = " << cb.mat << "}";
}

ostream& operator<<(ostream& os, const DirectionalLight& l)
{
    os << "{amb = " << l.ambient << ", dif = " << l.diffuse << ", spe = " << l.specular << ", dir = " << l.direction
       << "}";
    return os;
}

ostream& operator<<(ostream& os, const PointLight& l)
{
    os << "{amb = " << l.ambient << ", dif = " << l.diffuse << ", spe = " << l.specular << ", pos = " << l.pos
       << ", range = " << l.range << ", attenuation = " << l.attenuation << "}";
    return os;
}

ostream& operator<<(ostream& os, const SpotLight& l)
{
    os << "{amb = " << l.ambient << ", dif = " << l.diffuse << ", spe = " << l.specular << ", dir = " << l.direction
       << ", pos = " << l.pos << ", range = " << l.range << ", attenuation = " << l.attenuation << ", spot = " << l.spot
       << "}";
    return os;
}
