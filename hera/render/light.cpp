// hera
// Copyright (C) 2024  Cole Reynolds
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

#include <glm/ext/matrix_transform.hpp>

#include <hera/render/light.hpp>

namespace hera {

void PointLight::draw(Frame& f, float) const
{
    f->pipeline().uniform("model", model);
    vbuf.draw();
}

void PointLight::load_into(const string& root, const gl::Pipeline& prog) const
{
    prog.uniform(root + ".position", position);
    prog.uniform(root + ".constant", constant);
    prog.uniform(root + ".linear", linear);
    prog.uniform(root + ".quadratic", quadratic);
    prog.uniform(root + ".ambient", ambient);
    prog.uniform(root + ".diffuse", diffuse);
    prog.uniform(root + ".specular", specular);
}

void DirLight::load_into(const string& root, const gl::Pipeline& prog) const
{
    prog.uniform(root + ".direction", direction);
    prog.uniform(root + ".ambient", ambient);
    prog.uniform(root + ".diffuse", diffuse);
    prog.uniform(root + ".specular", specular);
}

} // namespace hera
