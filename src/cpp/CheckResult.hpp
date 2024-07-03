#pragma once

#include <stdexcept>
#include <string>

template <class Result>
void check(const Result& result, string msg)
{
    if (not result)
        throw runtime_error{msg};
}
