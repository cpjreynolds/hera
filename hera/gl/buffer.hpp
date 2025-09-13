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

#ifndef HERA_GL_BUFFER
#define HERA_GL_BUFFER

#include <hera/gl/object.hpp>

namespace hera::gl {

class VertexBuffer : object<id::varray{1}, id::buffer{2}> {
public:
    static constexpr id::varray vaoID{0};
    static constexpr id::buffer vboID{0}, eboID{1};
    gl_t ebo_type{0};
    GLsizei ebo_count{0};
    GLsizei vbo_count{0};

    constexpr id::varray vao() const { return get<vaoID>(); }
    constexpr id::buffer vbo() const { return get<vboID>(); }
    constexpr id::buffer ebo() const { return get<eboID>(); }

    VertexBuffer() = default;

    template<spanner R>
        requires is_vertex<range_v<R>>
    void data(const R& vertices, buffer_use usage = buffer_use::static_draw)
    {
        vbo_count = ranges::size(vertices);

        gl::bind(vao());
        gl::bind(vbo(), buffer_t::array);
        gl::allocate(buffer_t::array, vertices, usage);

        for (const auto& attr : vertex<range_v<R>>::format) {
            glVertexAttribPointer(attr.index, attr.size, attr.type, GL_FALSE,
                                  attr.stride, (GLvoid*)attr.offset);
            glEnableVertexAttribArray(attr.index);
        }
        gl::unbind(buffer_t::array);
    }

    template<spanner T, spanner I>
        requires is_vertex<range_v<T>> && gl_index_type<range_v<I>>
    void data(const T& vertices, const I& indices,
              buffer_use usage = buffer_use::static_draw)
    {
        ebo_type = gl_typeof<range_v<I>>();
        ebo_count = ranges::size(indices);
        vbo_count = ranges::size(vertices);

        gl::bind(vao());
        gl::bind(vbo(), buffer_t::array);
        gl::allocate(buffer_t::array, vertices, usage);
        gl::bind(ebo(), buffer_t::element_array);
        gl::allocate(buffer_t::element_array, indices, usage);

        for (const auto& attr : vertex<range_v<T>>::format) {
            glVertexAttribPointer(attr.index, attr.size, attr.type, GL_FALSE,
                                  attr.stride, (GLvoid*)attr.offset);
            glEnableVertexAttribArray(attr.index);
        }
        unbind();
        gl::unbind(buffer_t::array);
        gl::unbind(buffer_t::element_array);
    }

    template<spanner R>
        requires is_vertex<range_v<R>>
    VertexBuffer(const R& vertices, buffer_use usage = buffer_use::static_draw)
    {
        data(vertices, usage);
    }

    template<spanner T, spanner I>
        requires is_vertex<range_v<T>> && gl_index_type<range_v<I>>
    VertexBuffer(const T& vertices, const I& indices,
                 buffer_use usage = buffer_use::static_draw)
    {
        data(vertices, indices, usage);
    }

    void bind() const { gl::bind(vao()); }
    void unbind() const { gl::unbind<id::varray>(); }
    bool is_bound() const { return vao() == gl::current<id::varray>(); }

    void draw(primitive_t mode = primitive_t::triangles) const
    {
        bind();
        if (ebo_type) {
            gl::draw(mode, ebo_count, ebo_type);
        }
        else if (vbo_count != 0) {
            gl::draw(mode, vbo_count);
        }
        else {
            throw gl_error("attempt to draw null vertex buffer");
        }
        unbind();
    }
};

} // namespace hera::gl

#endif
