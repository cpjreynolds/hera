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

#ifndef HERA_GL_OBJECT_HPP
#define HERA_GL_OBJECT_HPP

#include <hera/common.hpp>
#include <hera/gl/common.hpp>
#include <hera/error.hpp>
#include <hera/log.hpp>
#include <hera/utility.hpp>
#include <hera/gl/vertex.hpp>

namespace hera::gl {

namespace detail {
constexpr bind_query to_bind_query(buffer_t b)
{
    using enum buffer_t;
    using enum bind_query;
    switch (b) {
    case array:
        return array_buffer;
    case element_array:
        return element_array_buffer;
    case pixel_pack:
        return pixel_pack_buffer;
    case pixel_unpack:
        return pixel_unpack_buffer;
    case texture:
        return texture_buffer;
    case transform_feedback:
        return transform_feedback_buffer;
    case uniform:
        return uniform_buffer;
    case copy_read:
    case copy_write:
    default:
        throw gl_error("bad buffer_t query");
    }
}
constexpr bind_query to_bind_query(texture_t t)
{
    using enum texture_t;
    using enum bind_query;
    switch (t) {
    case oneD:
        return texture_1d;
    case twoD:
        return texture_2d;
    case threeD:
        return texture_3d;
    case array_1d:
        return texture_1d_array;
    case array_2d:
        return texture_2d_array;
    case rectangle:
        return texture_rectangle;
    case cube_map:
        return texture_cube_map;
    case multisample_2d:
        return texture_2d_multisample;
    case multisample_2d_array:
        return texture_2d_multisample_array;
    case buffer:
        return texture_buffer;
    default:
        throw gl_error("bad texture_t query");
    }
};
template<typename T>
inline T binding(bind_query query)
{
    GLint store;
    glGetIntegerv(+query, &store);
    return static_cast<T>(store);
}
} // namespace detail

// returns the currently bound object.
template<typename T>
inline T current()
{
    using detail::binding;
    using enum bind_query;
    if constexpr (same_as<T, texture_u>) {
        return binding<texture_u>(active_texture);
    }
    else if constexpr (same_as<T, id::varray>) {
        return binding<id::varray>(vertex_array);
    }
    else if constexpr (same_as<T, id::pipeline>) {
        return binding<id::pipeline>(pipeline);
    }
    else if constexpr (same_as<T, id::renderbuffer>) {
        return binding<id::renderbuffer>(renderbuffer);
    }
    else {
        static_assert(false, "bad current target type");
    }
}
// returns buffer bound to target.
inline id::buffer current(buffer_t tgt)
{
    using namespace detail;
    return binding<id::buffer>(to_bind_query(tgt));
}
// returns texture bound to target.
inline id::texture current(texture_t tgt)
{
    using namespace detail;
    return binding<id::texture>(to_bind_query(tgt));
}
// returns framebuffer bound to target.
inline id::framebuffer current(framebuffer_t tgt)
{
    using detail::binding;
    using enum bind_query;
    switch (tgt) {
    case framebuffer_t::draw:
        return binding<id::framebuffer>(draw_framebuffer);
    case framebuffer_t::read:
        return binding<id::framebuffer>(read_framebuffer);
    default:
        throw gl_error("bad framebuffer_t");
    }
}

// unbinds the currently bound object type.
template<typename T>
inline void unbind()
{
    if constexpr (same_as<T, texture_u>) {
        glActiveTexture(GL_TEXTURE0);
    }
    else if constexpr (same_as<T, id::varray>) {
        glBindVertexArray(0);
    }
    else if constexpr (same_as<T, id::pipeline>) {
        glBindProgramPipeline(0);
    }
    else if constexpr (same_as<T, id::framebuffer>) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else if constexpr (same_as<T, id::renderbuffer>) {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    else {
        static_assert(false, "bad unbind target type");
    }
}
// unbinds the buffer target.
inline void unbind(buffer_t tgt)
{
    glBindBuffer(+tgt, 0);
}
// unbinds the indexed buffer target.
inline void unbind(buffer_t tgt, GLuint index)
{
    glBindBufferBase(+tgt, index, 0);
}
// unbinds the texture target.
inline void unbind(texture_t tgt)
{
    glBindTexture(+tgt, 0);
}
// unbinds the framebuffer target.
inline void unbind(framebuffer_t tgt)
{
    glBindFramebuffer(+tgt, 0);
}

// activates the texture unit.
inline void bind(texture_u unit)
{
    glActiveTexture(unit.offset());
}
// binds the vertex array object.
inline void bind(id::varray v)
{
    glBindVertexArray(v);
}
// binds the buffer object to the target.
inline void bind(id::buffer buf, buffer_t tgt)
{
    glBindBuffer(+tgt, buf);
}
// binds the buffer object to the indexed target.
inline void bind(id::buffer buf, buffer_t tgt, GLuint index)
{
    glBindBufferBase(+tgt, index, buf);
}
// binds a range within a buffer to the indexed target.
inline void bind(id::buffer buf, buffer_t tgt, GLuint index, GLintptr offset,
                 GLsizeiptr size)
{
    glBindBufferRange(+tgt, index, buf, offset, size);
}
// binds the texture object to the target.
inline void bind(id::texture tex, texture_t tgt)
{
    glBindTexture(+tgt, tex);
}
// binds the pipeline object.
inline void bind(id::pipeline pipe)
{
    glBindProgramPipeline(pipe);
}
// binds the framebuffer object to both read and write targets.
inline void bind(id::framebuffer fb)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
}
// binds the framebuffer object to the target.
inline void bind(id::framebuffer fb, framebuffer_t tgt)
{
    glBindFramebuffer(+tgt, fb);
}
// binds the renderbuffer object.
inline void bind(id::renderbuffer rb)
{
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
}

template<typename T>
struct object_traits;

template<>
struct object_traits<id::varray> {
    static void generate(int n, GLuint* dst) { glGenVertexArrays(n, dst); }
    static void destroy(int n, const GLuint* src)
    {
        glDeleteVertexArrays(n, src);
    }
    static constexpr string_view name = "vertex array";
};

template<>
struct object_traits<id::buffer> {
    static void generate(int n, GLuint* dst) { glGenBuffers(n, dst); }
    static void destroy(int n, const GLuint* src) { glDeleteBuffers(n, src); }
    static constexpr string_view name = "buffer";
};

template<>
struct object_traits<id::texture> {
    static void generate(int n, GLuint* dst) { glGenTextures(n, dst); }
    static void destroy(int n, const GLuint* src) { glDeleteTextures(n, src); }
    static constexpr string_view name = "texture";
};

template<>
struct object_traits<id::pipeline> {
    static void generate(int n, GLuint* dst) { glGenProgramPipelines(n, dst); }
    static void destroy(int n, const GLuint* src)
    {
        glDeleteProgramPipelines(n, src);
    }
    static constexpr string_view name = "pipeline";
};

template<>
struct object_traits<id::program> {
    static void generate(int n, GLuint* dst)
    {
        std::generate_n(dst, n, glCreateProgram);
    }
    static void destroy(int n, const GLuint* src)
    {
        std::for_each_n(src, n, glDeleteProgram);
    }
    static constexpr string_view name = "program";
};

template<>
struct object_traits<id::framebuffer> {
    static void generate(int n, GLuint* dst) { glGenFramebuffers(n, dst); }
    static void destroy(int n, const GLuint* src)
    {
        glDeleteFramebuffers(n, src);
    }
    static constexpr string_view name = "framebuffer";
};

template<>
struct object_traits<id::renderbuffer> {
    static void generate(int n, GLuint* dst) { glGenRenderbuffers(n, dst); }
    static void destroy(int n, const GLuint* src)
    {
        glDeleteRenderbuffers(n, src);
    }
    static constexpr string_view name = "renderbuffer";
};

// generic operations for all objects

template<typename T>
concept generatable =
    requires(int n, GLuint* dst) { object_traits<T>::generate(n, dst); };

template<typename T>
concept destroyable =
    requires(int n, const GLuint* src) { object_traits<T>::destroy(n, src); };

template<spanner R, generatable T = range_v<R>>
void generate(R& dest_r)
{
    auto dest =
        reinterpret_cast<std::underlying_type_t<T>*>(ranges::data(dest_r));
    auto n = ranges::size(dest_r);

    object_traits<T>::generate(n, dest);

    string_view plural = (n > 1 ? "s" : "");
    LOG_TRACE_L2("glGen {}{}: {}", object_traits<T>::name, plural,
                 vector(dest, dest + n));
}

template<generatable T>
void generate(T& dest)
{
    generate(views::single(dest));
}

template<spanner R, destroyable T = range_v<R>>
void destroy(R& src_r)
{
    auto src =
        reinterpret_cast<std::underlying_type_t<T>*>(ranges::cdata(src_r));
    auto n = ranges::size(src_r);

    object_traits<T>::destroy(n, src);

    string_view plural = (n > 1 ? "s" : "");
    LOG_TRACE_L2("glDel {}{}: {}", object_traits<T>::name, plural,
                 vector(src, src + n));
}

template<destroyable T>
void destroy(T& src)
{
    destroy(views::single(src));
}

// specific object operations

// object parameter get/set

// get buffer parameter.
template<typename T = GLint>
    requires std::convertible_to<GLint, T> ||
             (std::convertible_to<GLint64, T> && sizeof(T) == sizeof(GLint64))
T parameter(buffer_t tgt, GLenum pname)
{
    if constexpr (sizeof(T) == sizeof(GLint64)) {
        GLint64 param;
        glGetBufferParameteri64v(+tgt, pname, &param);
        return param;
    }
    else {
        GLint param;
        glGetBufferParameteriv(+tgt, pname, &param);
        return param;
    }
}

// set texture parameter.
template<typename T>
    requires same_as<T, GLint> || same_as<T, GLfloat> || gl_vector<T, 4>
void parameter(texture_t tgt, GLenum pname, const T& param)
{
    if constexpr (gl_vector<T, 4, int>) {
        glTexParameteriv(+tgt, pname, uniform_storage(param));
    }
    else if constexpr (gl_vector<T, 4, float>) {
        glTexParameterfv(+tgt, pname, uniform_storage(param));
    }
    else if constexpr (std::integral<T>) {
        glTexParameteri(+tgt, pname, param);
    }
    else if constexpr (std::floating_point<T>) {
        glTexParameterf(+tgt, pname, param);
    }
    else {
        assert(!"unexpected param type");
    }
}

// get texture parameter.
template<typename T = GLint>
    requires(std::convertible_to<GLint, T> ||
             std::convertible_to<GLfloat, T>) ||
            gl_vector<T, 4>
T parameter(texture_t tgt, GLenum pname)
{
    if constexpr (gl_vector<T, 4, int>) {
        T param;
        glGetTexParameteriv(+tgt, pname, uniform_storage(param));
        return param;
    }
    else if constexpr (gl_vector<T, 4, float>) {
        T param;
        glGetTexParameterfv(+tgt, pname, uniform_storage(param));
        return param;
    }
    else if constexpr (std::integral<T>) {
        GLint param;
        glGetTexParameteriv(+tgt, pname, &param);
        return param;
    }
    else if constexpr (std::floating_point<T>) {
        GLfloat param;
        glGetTexParameterfv(+tgt, pname, &param);
        return param;
    }
    else {
        assert(!"unexpected param type");
    }
}

// get program pipeline parameter.
inline GLint parameter(id::pipeline pipe, GLenum pname)
{
    GLint param;
    glGetProgramPipelineiv(pipe, pname, &param);
    return param;
}

// get program parameter.
inline GLint parameter(id::program prog, GLenum pname)
{
    GLint param;
    glGetProgramiv(prog, pname, &param);
    return param;
}

// set program parameter.
inline void parameter(id::program prog, GLenum pname, GLint val)
{
    glProgramParameteri(prog, pname, val);
}

// get framebuffer attachment parameter.
inline GLint parameter(framebuffer_t tgt, GLenum attach, GLenum pname)

{
    GLint param;
    glGetFramebufferAttachmentParameteriv(+tgt, attach, pname, &param);
    return param;
}

// get renderbuffer parameter.
inline GLint parameter(id::renderbuffer, GLenum pname)
{
    GLint param;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, pname, &param);
    return param;
}

// buffer/texture memory operations

// 1D texture allocation.
inline void allocate(texture_t tgt, internal_f internal, int w)
{
    assert(gl_dimensions(tgt) == 1);
    glTexImage1D(+tgt, 0, +internal, w, 0, 0, 0, nullptr);
}

// 2D texture allocation.
inline void allocate(texture_t tgt, internal_f internal, int w, int h)
{
    assert(gl_dimensions(tgt) == 2);
    glTexImage2D(+tgt, 0, +internal, w, h, 0, 0, 0, nullptr);
}

// 3D texture allocation.
inline void allocate(texture_t tgt, internal_f internal, int w, int h, int d)
{
    assert(gl_dimensions(tgt) == 3);
    glTexImage3D(+tgt, 0, +internal, w, h, d, 0, 0, 0, nullptr);
}

// 1D texture allocation with data.
template<spanner R>
    requires gl_type<range_v<R>>
void allocate(texture_t tgt, internal_f internalf, int w, const R& pixels,
              pixel_f pixelf = {}, pixel_t pixelty = gl_typeof<range_v<R>>())
{
    assert(gl_dimensions(tgt) == 1);
    if (pixelf == pixel_f::none) {
        pixelf = pixel_f{internalf};
    }
    glTexImage1D(+tgt, 0, +internalf, w, 0, +pixelf, +pixelty,
                 ranges::cdata(pixels));
}

// 2D texture allocation with data.
template<spanner R>
    requires gl_type<range_v<R>>
void allocate(texture_t tgt, internal_f internalf, int w, int h,
              const R& pixels, pixel_f pixelf = {},
              pixel_t pixelty = gl_typeof<range_v<R>>())
{
    assert(gl_dimensions(tgt) == 2);
    if (pixelf == pixel_f{0}) {
        pixelf = pixel_f{internalf};
    }
    glTexImage2D(+tgt, 0, +internalf, w, h, 0, +pixelf, +pixelty,
                 ranges::cdata(pixels));
}

// 3D texture allocation with data.
template<spanner R>
    requires gl_type<range_v<R>>
void allocate(texture_t tgt, internal_f internalf, int w, int h, int d,
              const R& pixels, pixel_f pixelf = {},
              pixel_t pixelty = gl_typeof<range_v<R>>())
{
    assert(gl_dimensions(tgt) == 3);
    if (pixelf == pixel_f{}) {
        pixelf = pixel_f{internalf};
    }
    glTexImage3D(+tgt, 0, +internalf, w, h, d, 0, +pixelf, +pixelty,
                 ranges::cdata(pixels));
}

// renderbuffer storage allocation
inline void allocate(id::renderbuffer, internal_f internalf, int w, int h)
{
    glRenderbufferStorage(GL_RENDERBUFFER, +internalf, w, h);
}

// update a segment of a 1D texture.
template<spanner R>
    requires gl_type<range_v<R>>
void data(texture_t tgt, int xoff, int w, const R& pixels, pixel_f pixelf,
          pixel_t pixelty = gl_typeof<range_v<R>>())
{
    assert(gl_dimensions(tgt) == 1);
    glTexSubImage1D(+tgt, 0, xoff, w, +pixelf, +pixelty, ranges::cdata(pixels));
}

// update a segment of a 2D texture.
template<spanner R>
    requires gl_type<range_v<R>>
void data(texture_t tgt, int xoff, int yoff, int w, int h, const R& pixels,
          pixel_f pixelf, pixel_t pixelty = gl_typeof<range_v<R>>())
{
    assert(gl_dimensions(tgt) == 2);
    glTexSubImage2D(+tgt, 0, xoff, yoff, w, h, +pixelf, +pixelty,
                    ranges::cdata(pixels));
}

// update a segment of a 3D texture.
template<spanner R>
    requires gl_type<range_v<R>>
void data(texture_t tgt, int xoff, int yoff, int zoff, int w, int h, int d,
          const R& pixels, pixel_f pixelf,
          pixel_t pixelty = gl_typeof<range_v<R>>())
{
    assert(gl_dimensions(tgt) == 3);
    glTexSubImage3D(+tgt, 0, xoff, yoff, zoff, w, h, d, +pixelf, +pixelty,
                    ranges::cdata(pixels));
}

// buffer allocation
inline void allocate(buffer_t tgt, GLsizeiptr size,
                     buffer_use use = buffer_use::static_draw)
{
    glBufferData(+tgt, size, nullptr, +use);
}

// buffer allocation with data
template<spanner R>
    requires gl_type<range_v<R>> || is_vertex<range_v<R>> ||
             glsl_type<range_v<R>>
void allocate(buffer_t tgt, const R& data,
              buffer_use use = buffer_use::static_draw)
{
    glBufferData(+tgt, size_bytes(data), ranges::cdata(data), +use);
}

// buffer data write
template<spanner R>
    requires gl_type<range_v<R>> || is_vertex<range_v<R>> ||
             glsl_type<range_v<R>>
void data(buffer_t tgt, GLintptr off, const R& data)
{
    glBufferSubData(+tgt, off, size_bytes(data), ranges::cdata(data));
}

// copy buffer data
inline void copy(buffer_t from, buffer_t to, GLintptr roff, GLintptr woff,
                 GLsizeiptr size)
{
    glCopyBufferSubData(+from, +to, roff, woff, size);
}

// define a 1D texture with pixels from READ_BUFFER.
inline void copy(texture_t tgt, internal_f iformat, int x, int y, int width)
{
    glCopyTexImage1D(+tgt, 0, +iformat, x, y, width, 0);
}

// define a 2D texture with pixels from READ_BUFFER.
inline void copy(texture_t tgt, internal_f iformat, int x, int y, int width,
                 int height)
{
    glCopyTexImage2D(+tgt, 0, +iformat, x, y, width, height, 0);
}

// replace a portion of 1D texture with pixels from READ_BUFFER.
inline void copy(texture_t tgt, int xoff, int x, int y, int width)
{
    glCopyTexSubImage1D(+tgt, 0, xoff, x, y, width);
}

// replace a portion of 2D texture with pixels from READ_BUFFER.
inline void copy(texture_t tgt, int xoff, int yoff, int x, int y, int width,
                 int height)
{
    glCopyTexSubImage2D(+tgt, 0, xoff, yoff, x, y, width, height);
}

// replace a portion of 3D texture with pixels from READ_BUFFER.
inline void copy(texture_t tgt, int xoff, int yoff, int zoff, int x, int y,
                 int width, int height)
{
    glCopyTexSubImage3D(+tgt, 0, xoff, yoff, zoff, x, y, width, height);
}

// copy READ_FRAMEBUFFER to DRAW_FRAMEBUFFER
inline void blit(int srcX0, int srcY0, int srcX1, int srcY1, int dstX0,
                 int dstY0, int dstX1, int dstY1,
                 GLenum mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR)
{
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1,
                      mask, filter);
}

// read buffer into main memory.
template<spanner R>
    requires std::is_trivially_copyable_v<range_v<R>>
void read(buffer_t tgt, GLintptr off, R& dest)
{
    glGetBufferSubData(+tgt, off, size_bytes(dest), ranges::data(dest));
}

// read texture into main memory.
template<spanner R>
    requires gl_type<range_v<R>>
void read(texture_t tgt, R& dest, pixel_f pixelf,
          pixel_t pixelty = gl_typeof<range_v<R>>())
{
    glGetTexImage(+tgt, 0, +pixelf, +pixelty, ranges::data(dest));
}

// read pixels from READ_FRAMEBUFFER into main memory.
template<spanner R>
    requires gl_type<range_v<R>>
void read(int x, int y, int w, int h, R& dest, GLenum pixelf,
          GLenum pixelty = gl_typeof<range_v<R>>())
{
    glReadPixels(x, y, w, h, pixelf, pixelty, ranges::data(dest));
}

// buffer mapping

// map a buffer.
inline void* bmap(buffer_t tgt, buffer_access access)
{
    return glMapBuffer(+tgt, +access);
}

// map a segment of a buffer.
inline void* bmap(buffer_t tgt, GLintptr offset, GLsizeiptr len,
                  map_flag access)
{
    return glMapBufferRange(+tgt, offset, len, +access);
}

// unmap a mapped buffer.
inline bool bunmap(buffer_t tgt)
{
    return glUnmapBuffer(+tgt);
}

// flush segment of mapped buffer.
inline void flush(buffer_t tgt, GLintptr offset, GLsizeiptr len)
{
    glFlushMappedBufferRange(+tgt, offset, len);
}

// attach renderbuffer to framebuffer.
inline void attach(framebuffer_t tgt, GLenum attachment, id::renderbuffer rb)
{
    glFramebufferRenderbuffer(+tgt, attachment, GL_RENDERBUFFER, rb);
}

// detach a buffer from a framebuffer.
inline void detach(framebuffer_t tgt, GLenum attachment)
{
    glFramebufferRenderbuffer(+tgt, attachment, GL_RENDERBUFFER, 0);
}

// attach texture to framebuffer.
inline void attach(framebuffer_t tgt, GLenum attachment, id::texture tex,
                   int level = 0)
{
    glFramebufferTexture(+tgt, attachment, tex, level);
}

// attach texture to framebuffer.
inline void attach(framebuffer_t tgt, GLenum attachment, id::texture tex,
                   int level, int layer)
{
    glFramebufferTextureLayer(+tgt, attachment, tex, level, layer);
}

// attach program to pipeline
inline void attach(id::pipeline pipe, id::program prog, shader_t stage)
{
    glUseProgramStages(pipe, stage.bit(), prog);
}

inline void detach(id::pipeline pipe, shader_t stage)
{
    glUseProgramStages(pipe, stage.bit(), 0);
}

inline void draw(primitive_t mode, GLsizei count, GLint start = 0)
{
    glDrawArrays(+mode, start, count);
}

inline void draw(primitive_t mode, GLsizei count, gl_t type, size_t offset = 0)
{
    glDrawElements(+mode, count, +type, (const void*)offset); // NOLINT
}

// set the value of a uniform variable.
template<uniformable U>
void uniform(id::program p, GLint loc, const U& v)
{
    using T = uniform_traits<U>::value_type;
    auto ptr = uniform_traits<U>::storage(v);
    auto size = uniform_traits<U>::size(v);
    if constexpr (same_as<T, float>) {
        glProgramUniform1fv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, uint32_t>) {
        glProgramUniform1uiv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, int32_t>) {
        glProgramUniform1iv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, vec2>) {
        glProgramUniform2fv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, vec3>) {
        glProgramUniform3fv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, vec4>) {
        glProgramUniform4fv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, uvec2>) {
        glProgramUniform2uiv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, uvec3>) {
        glProgramUniform3uiv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, uvec4>) {
        glProgramUniform4uiv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, ivec2>) {
        glProgramUniform2iv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, ivec3>) {
        glProgramUniform3iv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, ivec4>) {
        glProgramUniform4iv(p, loc, size, ptr);
    }
    else if constexpr (same_as<T, mat2>) {
        glProgramUniformMatrix2fv(p, loc, size, false, ptr);
    }
    else if constexpr (same_as<T, mat3>) {
        glProgramUniformMatrix3fv(p, loc, size, false, ptr);
    }
    else if constexpr (same_as<T, mat4>) {
        glProgramUniformMatrix4fv(p, loc, size, false, ptr);
    }
    else if constexpr (same_as<T, mat2x3>) {
        glProgramUniformMatrix2x3fv(p, loc, size, false, ptr);
    }
    else if constexpr (same_as<T, mat2x4>) {
        glProgramUniformMatrix2x4fv(p, loc, size, false, ptr);
    }
    else if constexpr (same_as<T, mat3x2>) {
        glProgramUniformMatrix3x2fv(p, loc, size, false, ptr);
    }
    else if constexpr (same_as<T, mat3x4>) {
        glProgramUniformMatrix3x4fv(p, loc, size, false, ptr);
    }
    else if constexpr (same_as<T, mat4x2>) {
        glProgramUniformMatrix4x2fv(p, loc, size, false, ptr);
    }
    else if constexpr (same_as<T, mat4x3>) {
        glProgramUniformMatrix4x3fv(p, loc, size, false, ptr);
    }
    else {
        throw hera::runtime_error("bad type");
    }
}

namespace detail {

class refcount {
    static constexpr uint32_t sentinel = std::numeric_limits<uint32_t>::max();

    inline static uint32_t free_head = sentinel;
    inline static vector<uint32_t> rcs{};

    uint32_t _idx;
    uint32_t inc() noexcept { return ++rcs[_idx]; };
    uint32_t dec() noexcept { return --rcs[_idx]; };

    static uint32_t acquire()
    {
        if (free_head == sentinel) {
            // no free space.
            rcs.emplace_back(1);
            return rcs.size() - 1;
        }
        else {
            uint32_t rv = free_head;
            free_head = rcs[free_head];
            return rv;
        }
    }

    static void release(uint32_t idx) noexcept
    {
        rcs[idx] = free_head;
        free_head = idx;
    }

public:
    explicit constexpr refcount(nullptr_t) noexcept : _idx{sentinel} {};
    refcount() : _idx{acquire()} {};
    refcount(const refcount& other) noexcept : _idx{other._idx}
    {
        if (_idx != sentinel)
            inc();
    };
    refcount& operator=(const refcount& other) noexcept
    {
        refcount(other).swap(*this);
        return *this;
    }
    refcount(refcount&& other) noexcept
        : _idx{std::exchange(other._idx, sentinel)} {};
    refcount& operator=(refcount&& other) noexcept
    {
        refcount(std::move(other)).swap(*this);
        return *this;
    }
    uint32_t count() const noexcept
    {
        return _idx == sentinel ? sentinel : rcs[_idx];
    }
    bool unique() const noexcept { return count() == 1; }
    void swap(refcount& other) noexcept { std::swap(_idx, other._idx); }
    friend void swap(refcount& lhs, refcount& rhs) noexcept { lhs.swap(rhs); }
    friend auto operator<=>(const refcount&,
                            const refcount&) noexcept = default;
    ~refcount()
    {
        if (_idx != sentinel && dec() == 0) {
            release(_idx);
        }
    }
};

/*
class refcount {
    long* _ctr;
    long inc() noexcept { return ++*_ctr; }
    long dec() noexcept { return --*_ctr; }

public:
    explicit constexpr refcount(nullptr_t) noexcept : _ctr{nullptr} {};
    refcount() : _ctr{new long{0}} {};
    refcount(const refcount& other) noexcept : _ctr{other._ctr}
    {
        if (_ctr)
            inc();
    };
    refcount& operator=(const refcount& other) noexcept
    {
        refcount(other).swap(*this);
        return *this;
    }
    refcount(refcount&& other) noexcept
        : _ctr{std::exchange(other._ctr, nullptr)} {};
    refcount& operator=(refcount&& other) noexcept
    {
        refcount(std::move(other)).swap(*this);
        return *this;
    }
    long count() const noexcept { return _ctr == nullptr ? -1 : *_ctr; }
    bool unique() const noexcept { return count() == 0; }
    void swap(refcount& other) noexcept { std::swap(_ctr, other._ctr); }
    friend void swap(refcount& lhs, refcount& rhs) noexcept { lhs.swap(rhs); }
    friend auto operator<=>(const refcount&,
                            const refcount&) noexcept = default;
    ~refcount()
    {
        if (_ctr && dec() == -1) {
            delete _ctr;
        }
    }
};
*/

template<id::id T, id::id auto... Ns>
struct type_offset;

template<id::id T, id::id auto N, id::id auto... Ns>
    requires same_as<T, decltype(N)>
struct type_offset<T, N, Ns...> : std::integral_constant<size_t, 0> {
    static constexpr T size = N;
};

template<id::id T, id::id auto N, id::id auto... Ns>
    requires(!same_as<T, decltype(N)>)
struct type_offset<T, N, Ns...>
    : std::integral_constant<size_t, N + (type_offset<T, Ns...>::value)> {
    static constexpr T size = type_offset<T, Ns...>::size;
};

template<id::id T, id::id auto N>
    requires(!same_as<T, decltype(N)>)
struct type_offset<T, N>;

} // namespace detail

// represents shared ownership of a fixed set of OpenGL object IDs.
template<id::id auto... IDs>
class object {
    detail::refcount rc;
    array<GLuint, (IDs + ... + 0)> id_data{};

    // protected:
public:
    object()
    {
        LOG_TRACE_L2("creating {}", pretty_type_name());
        auto genfn = [](spanner auto r) { gl::generate(r); };
        (genfn(get<decltype(IDs)>()), ...);
    }

    // support for creating a null object
    explicit object(nullptr_t) : rc{nullptr} {};

    object(const object& other) noexcept = default;
    object& operator=(const object& other) noexcept = default;
    object(object&& other) noexcept
        : rc{std::move(other.rc)},
          id_data{other.id_data}
    {
        other.id_data.fill(0);
    }
    object& operator=(object&& other) noexcept
    {
        object(std::move(other)).swap(*this);
        return *this;
    }

    // number of elements
    static consteval size_t size() { return (IDs + ... + 0); }

    template<id::id T>
    static constexpr bool contains =
        requires { detail::type_offset<T, IDs...>::size; };

    // number of elements of type T
    template<id::id T>
        requires contains<T>
    static consteval size_t size()
    {
        return detail::type_offset<T, IDs...>::size;
    };

    template<id::id auto N>
    static constexpr bool contained_in = N < object::size<decltype(N)>();

    // retrieves the `n`th ID of type T with runtime bounds-check.
    template<id::id T>
        requires contains<T>
    constexpr decltype(auto) at(this auto&& self, T n)
    {
        constexpr auto off = detail::type_offset<T, IDs...>::value;
        if (n >= size<T>()) {
            throw hera::runtime_error("object ID index out of range");
        }
        using ret_t = like_t<decltype(self), T>;
        using self_t = like_t<decltype(self), object>;
        return reinterpret_cast<ret_t>(((self_t)self).id_data[off + n]);
    }

    // retrieves the `n`th ID of type T with runtime bounds-check.
    template<id::id T>
        requires contains<T>
    constexpr decltype(auto) at(this auto&& self, std::integral auto n)
    {
        return self.at(T(n));
    }

    template<id::id auto N>
        requires contained_in<N>
    constexpr decltype(auto) get(this auto&& self) noexcept
    {
        constexpr auto off = detail::type_offset<decltype(N), IDs...>::value;
        using self_t = like_t<decltype(self), object>;
        using ret_t = like_t<decltype(self), decltype(N)>;
        return reinterpret_cast<ret_t>(((self_t)self).id_data[off + N]);
    }

    // same as `at(n)` but without bounds checking
    template<id::id T>
        requires contains<T>
    constexpr decltype(auto) operator[](this auto&& self, T n)
    {
        constexpr auto off = detail::type_offset<T, IDs...>::value;
        using self_t = like_t<decltype(self), object>;
        using ret_t = like_t<decltype(self), T>;
        return reinterpret_cast<ret_t>(((self_t)self).id_data[off + n]);
    }

    // retrieves all the IDs of type T.
    template<id::id T>
        requires contains<T>
    auto get(this auto&& self) noexcept
        -> span<remove_reference_t<like_t<decltype(self), T>>, size<T>()>
    {
        constexpr auto off = detail::type_offset<T, IDs...>::value;
        using self_t = like_t<decltype(self), object>;
        using ret_t = remove_reference_t<like_t<decltype(self), T>>;
        auto ptr = reinterpret_cast<ret_t*>(&((self_t)self).id_data[off]);
        return span<ret_t, size<T>()>{ptr, size<T>()};
    }

    // tuple-like get of the ID at index I.
    template<size_t I>
    friend std::tuple_element_t<I, object>& get(object& v) noexcept
    {
        using type = typename std::tuple_element<I, object>::type;
        return reinterpret_cast<type&>(v.id_data[I]);
    }
    // tuple-like get of the ID at index I.
    template<size_t I>
    friend std::tuple_element_t<I, object> const& get(const object& v) noexcept
    {
        using type = typename std::tuple_element<I, object>::type;
        return reinterpret_cast<const type&>(v.id_data[I]);
    }

    void swap(object& other) noexcept
    {
        rc.swap(other.rc);
        id_data.swap(other.id_data);
    }

    friend void swap(object& lhs, object& rhs) noexcept { lhs.swap(rhs); }
    friend auto operator<=>(const object&, const object&) noexcept = default;

    ~object()
    {
        if (rc.unique())
            destroy();
    }

private:
    void destroy() noexcept
    {
        LOG_TRACE_L2("destroying {}", pretty_type_name());
        auto delfn = [](spanner auto r) { gl::destroy(r); };
        (delfn(get<decltype(IDs)>()), ...);
        gl::checkerror();
    }

    static constexpr string pretty_type_name()
    {
        string raw = hera::type_of<object<IDs...>>("hera::gl::", "id::");

        for (auto loc = raw.find('('); loc != raw.npos;
             loc = raw.find('(', loc)) {
            raw.erase(loc, 1);
            loc = raw.find(')', loc);
            raw[loc++] = '[';
            while (std::isdigit(raw[loc])) {
                ++loc;
            }
            raw.insert(loc, 1, ']');
        }
        return raw;
    }
};

} // namespace hera::gl

template<size_t I, hera::gl::id::id auto N, hera::gl::id::id auto... Ns>
struct std::tuple_element<I, hera::gl::object<N, Ns...>>
    : std::conditional<(I < N),
                       std::tuple_element<0, hera::gl::object<N, Ns...>>,
                       std::tuple_element<(I - N), hera::gl::object<Ns...>>> {};

template<hera::gl::id::id auto N, hera::gl::id::id auto... Ns>
struct std::tuple_element<0, hera::gl::object<N, Ns...>> {
    using type = decltype(N);
};

template<hera::gl::id::id auto... Ns>
struct std::tuple_size<hera::gl::object<Ns...>>
    : std::integral_constant<size_t, (Ns + ... + 0)> {};

// formatting

template<hera::gl::id::id T>
struct fmt::formatter<T> : formatter<GLuint> {};

#endif
