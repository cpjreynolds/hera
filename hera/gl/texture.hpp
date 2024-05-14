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

#ifndef HERA_TEXTURE_HPP
#define HERA_TEXTURE_HPP

#include <hera/common.hpp>
#include <hera/error.hpp>
#include <hera/log.hpp>
#include <hera/gl/object.hpp>

namespace hera::gl {

struct TextureParams {
    GLint wrap_s = GL_REPEAT;
    GLint wrap_t = GL_REPEAT;
    GLint min_filter = GL_LINEAR;
    GLint mag_filter = GL_LINEAR;
    optional<ivec4> swizzle = nullopt;

    void apply(texture_t tgt) const
    {
        gl::parameter(tgt, GL_TEXTURE_WRAP_S, wrap_s);
        gl::parameter(tgt, GL_TEXTURE_WRAP_T, wrap_t);
        gl::parameter(tgt, GL_TEXTURE_MIN_FILTER, min_filter);
        gl::parameter(tgt, GL_TEXTURE_MAG_FILTER, mag_filter);
        if (swizzle) {
            gl::parameter(tgt, GL_TEXTURE_SWIZZLE_RGBA, *swizzle);
        }
    }
};

struct TextureArray : object<id::texture(1)> {
    static constexpr id::texture texID{0};
    static constexpr texture_t target = texture_t::array_2d;

    ivec3 size{0};
    internal_f iformat;
    mutable texture_u _unit = 3;

    constexpr id::texture id() const { return get<texID>(); }
    void bind(optional<texture_u> unit = nullopt) const
    {
        if (unit) {
            _unit = *unit;
        }
        gl::bind(_unit);
        gl::bind(id(), target);
    }

    void unit(texture_u v) const { _unit = v; }
    texture_u unit() const { return _unit; }

    void params(const TextureParams& params) { params.apply(target); }

    TextureArray() {};
    TextureArray(internal_f internalf, int w, int h, int n, texture_u unit = 0)
        : size{w, h, n},
          iformat{internalf},
          _unit{unit}
    {
        bind();
        gl::allocate(target, iformat, size.x, size.y, size.z);
    }

    template<spanner R>
        requires gl_type<range_v<R>>
    TextureArray(internal_f internalf, int w, int h, int n, const R& pixels,
                 pixel_f pixelf = {}, texture_u unit = 0)
        : size{w, h, n},
          iformat{internalf},
          _unit{unit}
    {
        if (!pixelf) {
            pixelf = pixel_f{iformat};
        }
        bind();
        gl::allocate(target, iformat, size.x, size.y, size.z, pixels, pixelf);
    }

    void allocate(internal_f internalf, int w, int h, int n)
    {
        size = ivec3{w, h, n};
        iformat = internalf;
        gl::allocate(target, iformat, size.x, size.y, size.z);
    }

    template<spanner R>
        requires gl_type<range_v<R>>
    void allocate(internal_f internalf, int w, int h, int n, const R& pixels,
                  pixel_f pixelf = {})
    {
        size = ivec3{w, h, n};
        iformat = internalf;
        if (pixelf == pixel_f{}) {
            pixelf = pixel_f{iformat};
        }
        gl::allocate(target, iformat, size.x, size.y, size.z, pixels, pixelf);
    }

    template<spanner R>
        requires gl_type<range_v<R>>
    void data(int idx, const R& pixels, pixel_f pixelf = {}) const
    {
        assert(size.x != 0 && size.y != 0 && size.z != 0);
        if (!pixelf) {
            pixelf = pixel_f{iformat};
        }
        const int z = gl_npixels(pixels, pixelf) / (size.x * size.y);
        gl::data(target, 0, 0, idx, size.x, size.y, z, pixels, pixelf);
    }
};

struct Texture : object<id::texture(1)> {
    static constexpr id::texture texID{0};
    static constexpr texture_t target = texture_t::twoD;
    mutable texture_u _unit{0};

    Texture(const path& fpath, texture_u unit = texture_u{0},
            const TextureParams& params = {});

    Texture(const Texture&) = default;
    Texture& operator=(const Texture&) = default;

    constexpr id::texture id() const { return get<texID>(); }

    void bind(optional<texture_u> unit = nullopt) const
    {
        if (unit) {
            _unit = *unit;
        }
        gl::bind(_unit);
        gl::bind(id(), target);
        gl::checkerror();
    }

    void unit(texture_u v) const { _unit = v; }
    texture_u unit() const { return _unit; }
    void unbind() const { gl::unbind(target); }
    bool is_bound() const { return id() == gl::current(target); }
};

} // namespace hera::gl

#endif
