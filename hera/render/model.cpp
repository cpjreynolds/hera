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

#include <hera/render/model.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace hera {

namespace {
Assimp::Importer& get_importer()
{
    static Assimp::Importer obj;
    return obj;
}
} // namespace

shared_ptr<Model> asset<Model>::load_from(const link& pat)
{
    // link dir = pat.parent_path();
    // link::push(pat);
    LOG_DEBUG("loading model: {}", pat);
    const aiScene* scene = get_importer().ReadFile(
        pat.resolve(), aiProcess_Triangulate | aiProcess_FlipUVs);
    LOG_DEBUG("model name: {}", scene->mName.C_Str());
    LOG_DEBUG("model root name: {}", scene->mRootNode->mName.C_Str());
    return nullptr;
}

} // namespace hera
