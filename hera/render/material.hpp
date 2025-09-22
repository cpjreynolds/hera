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

#ifndef HERA_RENDER_MATERIAL_HPP
#define HERA_RENDER_MATERIAL_HPP

#include <hera/assets.hpp>
#include <hera/gl/texture.hpp>
#include <hera/gl/program.hpp>
#include <hera/render/assimp_util.hpp>

#include <assimp/scene.h>

namespace hera {

struct Material {
    vec3 color_ambient{1.0};
    vec3 color_diffuse{1.0};
    vec3 color_specular{1.0};
    float shininess{1.0};
    gl::Texture2d tex_diffuse;
    gl::Texture2d tex_specular;

    Material() = default;
    Material(const aiMaterial*);
};

template<>
struct asset<Material[]> {
    shared_ptr<Material[]> load_from(const link& p);
};

struct Material2 {
    gl::Texture2d diffuse;
    gl::Texture2d specular;
    float shine;

    Material2(gl::Texture2d diff, gl::Texture2d spec, float shine)
        : diffuse{std::move(diff)},
          specular{std::move(spec)},
          shine{shine} {};

    void load_into(const string& root, const gl::Pipeline&) const;
};

} // namespace hera

#endif
