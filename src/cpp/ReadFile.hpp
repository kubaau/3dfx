#pragma once

#include <fstream>
#include <tools/Typedefs.hpp>
#include "CheckResult.hpp"
#include "IoTypedefs.hpp"

inline void jump(ifstream& file, ios_base::seekdir pos)
{
    file.seekg(0, pos);
}

inline void jumpToStart(ifstream& file)
{
    jump(file, ios_base::beg);
}

inline void jumpToEnd(ifstream& file)
{
    jump(file, ios_base::end);
}

inline auto readBinaryFile(FilePath path)
{
    const auto& path_str = path.string();

    ifstream file{path, ios::binary};
    check(file, "DXCount not load file " + path_str);

    jumpToEnd(file);
    IOBuffer data(static_cast<size_t>(file.tellg()));

    jumpToStart(file);
    file.read(data.data(), data.size());
    return data;
}

inline auto openFile(FilePath path)
{
    ifstream file{path};
    check(file, "Cannot open " + path.string());
    return file;
}
