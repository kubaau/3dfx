#pragma once

#include <unordered_map>
#include "IdTypedefs.hpp"

template <class T>
using Map = unordered_map<ID, T>;
