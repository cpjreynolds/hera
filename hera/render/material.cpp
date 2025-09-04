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

#include <hera/render/material.hpp>

namespace hera {

void Material::upload(const gl::Pipeline& prog, int idx) const
{
    string uname{"material"};

    if (idx != -1) {
        uname += std::format("[{}]", idx);
    }

    prog.uniform(uname + ".diff", static_cast<GLint>(+diff.unit()));
    prog.uniform(uname + ".spec", static_cast<GLint>(+spec.unit()));
    prog.uniform(uname + ".shine", shine);
    gl::checkerror();
}

} // namespace hera
