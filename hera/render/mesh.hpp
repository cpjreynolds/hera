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

#ifndef HERA_RENDER_MESH
#define HERA_RENDER_MESH

#include <hera/common.hpp>
#include <hera/gl/vertex.hpp>

#include <assimp/scene.h>

namespace hera {

class Mesh {
    struct vertex {
        vec3 position;
        vec3 normal;
        vec2 tex;
    };

    vector<vertex> vertices;
    vector<uint32_t> indices;

    Mesh(const aiMesh*, const aiScene*);
};

template<>
struct gl::vertex<Mesh::vertex> : gl::attributes<vec3, vec3, vec2> {};

} // namespace hera

#endif
