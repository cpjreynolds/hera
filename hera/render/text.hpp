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

#ifndef HERA_RENDER_TEXT_HPP
#define HERA_RENDER_TEXT_HPP

#include <hera/common.hpp>
#include <hera/error.hpp>
#include <hera/log.hpp>
#include <hera/utility.hpp>
#include <hera/input.hpp>
#include <hera/gl/program.hpp>
#include <hera/gl/texture.hpp>
#include <hera/gl/buffer.hpp>
#include <hera/gl/object.hpp>

namespace hera {

struct Alphabet {
    ivec2 size;
    ivec2 extents;
    ivec2 padded;
    vector<unsigned char> buffer;
    // char -> texture index
    hash_map<char32_t, int> charmap;
    gl::TextureArray texarray;

    Alphabet(const Config&);

    void bind(gl::texture_u unit = 3) const { texarray.bind(unit); }
};

struct Scribe {
    gl::VertexBuffer vbuf;
    Alphabet alphabet;

    Scribe(const Config&);

    void put(char, float x, float y, const gl::Pipeline&) const;
    void put(char32_t, float x, float y, const gl::Pipeline&) const;

    void put(string_view, float x, float y, const gl::Pipeline&) const;
};

} // namespace hera

#endif
