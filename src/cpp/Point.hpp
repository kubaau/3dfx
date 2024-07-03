#pragma once

#include <iostream>

struct Point
{
    int x, y;
};

inline auto& operator<<(ostream& os, Point p)
{
    return os << "{" << p.x << ", " << p.y << "}";
}
