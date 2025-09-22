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

#include <hera/render/material.hpp>

namespace hera {

void Material2::load_into(const string& root, const gl::Pipeline& prog) const
{
    prog.uniform(root + ".diffuse", diffuse.unit());
    prog.uniform(root + ".specular", specular.unit());
    prog.uniform(root + ".shine", shine);
    gl::checkerror();
}

Material::Material(const aiMaterial* mat)
{
    aiColor3D color;
    if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
        color_ambient = to_glm(color);
    }
    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
        color_diffuse = to_glm(color);
    }
    if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
        color_specular = to_glm(color);
    }
    mat->Get(AI_MATKEY_SHININESS, shininess);

    aiString pat;
    mat->GetTexture(aiTextureType_DIFFUSE, 0, &pat);
    link lnk{pat.C_Str()};
    tex_diffuse.allocate(lnk);

    mat->GetTexture(aiTextureType_SPECULAR, 0, &pat);
    lnk = {pat.C_Str()};
    tex_specular.allocate(lnk);
}

} // namespace hera
