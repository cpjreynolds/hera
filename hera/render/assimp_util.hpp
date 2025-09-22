// hera
// Copyright (C) 2024-2025  Cole Reynolds
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef HERA_RENDER_ASSIMP_UTIL
#define HERA_RENDER_ASSIMP_UTIL

#include <assimp/types.h>

#include <hera/common.hpp>

namespace hera {

constexpr vec3 to_glm(const aiVector3D& v)
{
    return {v.x, v.y, v.z};
}

constexpr mat4 to_glm(const aiMatrix4x4& m)
{
    return mat4{{m.a1, m.b1, m.c1, m.d1},
                {m.a2, m.b2, m.c2, m.d2},
                {m.a3, m.b3, m.c3, m.d3},
                {m.a4, m.b4, m.c4, m.d4}};
}

constexpr vec3 to_glm(const aiColor3D& v)
{
    return {v.r, v.g, v.b};
}

constexpr vec4 to_glm(const aiColor4D& v)
{
    return {v.r, v.g, v.b, v.a};
}

} // namespace hera

#endif
