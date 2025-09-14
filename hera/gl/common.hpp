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

#ifndef HERA_GL_COMMON_HPP
#define HERA_GL_COMMON_HPP

#include <source_location>

#include <hera/common.hpp>
#include <hera/utility.hpp>
#include <hera/error.hpp>
// NOLINTBEGIN(bugprone-macro-*,performance-enum-size)
namespace hera {
namespace gl {

struct gl_error : hera::runtime_error {
    using runtime_error::runtime_error;
};

// 'id's represent OpenGL objects.
namespace id {

enum buffer : GLuint {};
enum texture : GLuint {};
enum varray : GLuint {};
enum pipeline : GLuint {};
enum program : GLuint {};
enum framebuffer : GLuint {};
enum renderbuffer : GLuint {};

// partial specialization doesn't play nice with (same_as<T, Us...> || ...) so
// gotta use same_as a lot.

// specifies that a type is an OpenGL object ID.
template<typename T>
concept id = same_as<T, buffer> || same_as<T, texture> || same_as<T, varray> ||
             same_as<T, pipeline> || same_as<T, program> ||
             same_as<T, framebuffer> || same_as<T, renderbuffer>;

template<typename T, typename U>
concept compatible_with =
    id<T> && (same_as<std::underlying_type_t<T>, U> || same_as<T, U>);

} // namespace id

// namespace here is really just for vim syntax folding.
namespace {
#define HERA_GL_T(X)                                                           \
    X(none, GL_NONE)                                                           \
    X(i8, GL_BYTE)                                                             \
    X(u8, GL_UNSIGNED_BYTE)                                                    \
    X(i16, GL_SHORT)                                                           \
    X(u16, GL_UNSIGNED_SHORT)                                                  \
    X(i32, GL_INT)                                                             \
    X(u32, GL_UNSIGNED_INT)                                                    \
    X(f16, GL_HALF_FLOAT)                                                      \
    X(f32, GL_FLOAT)                                                           \
    X(f64, GL_DOUBLE)                                                          \
    X(vec2, GL_FLOAT_VEC2)                                                     \
    X(vec3, GL_FLOAT_VEC3)                                                     \
    X(vec4, GL_FLOAT_VEC4)                                                     \
    X(ivec2, GL_INT_VEC2)                                                      \
    X(ivec3, GL_INT_VEC3)                                                      \
    X(ivec4, GL_INT_VEC4)                                                      \
    X(uvec2, GL_UNSIGNED_INT_VEC2)                                             \
    X(uvec3, GL_UNSIGNED_INT_VEC3)                                             \
    X(uvec4, GL_UNSIGNED_INT_VEC4)                                             \
    X(dvec2, GL_DOUBLE_VEC2)                                                   \
    X(dvec3, GL_DOUBLE_VEC3)                                                   \
    X(dvec4, GL_DOUBLE_VEC4)                                                   \
    X(mat2, GL_FLOAT_MAT2)                                                     \
    X(mat2x3, GL_FLOAT_MAT2x3)                                                 \
    X(mat2x4, GL_FLOAT_MAT2x4)                                                 \
    X(mat3, GL_FLOAT_MAT3)                                                     \
    X(mat3x2, GL_FLOAT_MAT3x2)                                                 \
    X(mat3x4, GL_FLOAT_MAT3x4)                                                 \
    X(mat4, GL_FLOAT_MAT4)                                                     \
    X(mat4x2, GL_FLOAT_MAT4x2)                                                 \
    X(mat4x3, GL_FLOAT_MAT4x3)                                                 \
    X(dmat2, GL_DOUBLE_MAT2)                                                   \
    X(dmat2x3, GL_DOUBLE_MAT2x3)                                               \
    X(dmat2x4, GL_DOUBLE_MAT2x4)                                               \
    X(dmat3, GL_DOUBLE_MAT3)                                                   \
    X(dmat3x2, GL_DOUBLE_MAT3x2)                                               \
    X(dmat3x4, GL_DOUBLE_MAT3x4)                                               \
    X(dmat4, GL_DOUBLE_MAT4)                                                   \
    X(dmat4x2, GL_DOUBLE_MAT4x2)                                               \
    X(dmat4x3, GL_DOUBLE_MAT4x3)

#define HERA_GLSL_T(X)                                                         \
    X(none, GL_NONE)                                                           \
    X(i8, GL_BYTE)                                                             \
    X(u8, GL_UNSIGNED_BYTE)                                                    \
    X(i16, GL_SHORT)                                                           \
    X(u16, GL_UNSIGNED_SHORT)                                                  \
    X(i32, GL_INT)                                                             \
    X(u32, GL_UNSIGNED_INT)                                                    \
    X(f16, GL_HALF_FLOAT)                                                      \
    X(f32, GL_FLOAT)                                                           \
    X(f64, GL_DOUBLE)                                                          \
    X(vec2, GL_FLOAT_VEC2)                                                     \
    X(vec3, GL_FLOAT_VEC3)                                                     \
    X(vec4, GL_FLOAT_VEC4)                                                     \
    X(ivec2, GL_INT_VEC2)                                                      \
    X(ivec3, GL_INT_VEC3)                                                      \
    X(ivec4, GL_INT_VEC4)                                                      \
    X(uvec2, GL_UNSIGNED_INT_VEC2)                                             \
    X(uvec3, GL_UNSIGNED_INT_VEC3)                                             \
    X(uvec4, GL_UNSIGNED_INT_VEC4)                                             \
    X(dvec2, GL_DOUBLE_VEC2)                                                   \
    X(dvec3, GL_DOUBLE_VEC3)                                                   \
    X(dvec4, GL_DOUBLE_VEC4)                                                   \
    X(mat2, GL_FLOAT_MAT2)                                                     \
    X(mat2x3, GL_FLOAT_MAT2x3)                                                 \
    X(mat2x4, GL_FLOAT_MAT2x4)                                                 \
    X(mat3, GL_FLOAT_MAT3)                                                     \
    X(mat3x2, GL_FLOAT_MAT3x2)                                                 \
    X(mat3x4, GL_FLOAT_MAT3x4)                                                 \
    X(mat4, GL_FLOAT_MAT4)                                                     \
    X(mat4x2, GL_FLOAT_MAT4x2)                                                 \
    X(mat4x3, GL_FLOAT_MAT4x3)                                                 \
    X(dmat2, GL_DOUBLE_MAT2)                                                   \
    X(dmat2x3, GL_DOUBLE_MAT2x3)                                               \
    X(dmat2x4, GL_DOUBLE_MAT2x4)                                               \
    X(dmat3, GL_DOUBLE_MAT3)                                                   \
    X(dmat3x2, GL_DOUBLE_MAT3x2)                                               \
    X(dmat3x4, GL_DOUBLE_MAT3x4)                                               \
    X(dmat4, GL_DOUBLE_MAT4)                                                   \
    X(dmat4x2, GL_DOUBLE_MAT4x2)                                               \
    X(dmat4x3, GL_DOUBLE_MAT4x3)                                               \
    X(sampler1D, GL_SAMPLER_1D)                                                \
    X(sampler2D, GL_SAMPLER_2D)                                                \
    X(sampler3D, GL_SAMPLER_3D)                                                \
    X(samplerCube, GL_SAMPLER_CUBE)                                            \
    X(sampler1DShadow, GL_SAMPLER_1D_SHADOW)                                   \
    X(sampler2DShadow, GL_SAMPLER_2D_SHADOW)                                   \
    X(sampler1DArray, GL_SAMPLER_1D_ARRAY)                                     \
    X(sampler2DArray, GL_SAMPLER_2D_ARRAY)                                     \
    X(sampler1DArrayShadow, GL_SAMPLER_1D_ARRAY_SHADOW)                        \
    X(sampler2DArrayShadow, GL_SAMPLER_2D_ARRAY_SHADOW)                        \
    X(sampler2DMS, GL_SAMPLER_2D_MULTISAMPLE)                                  \
    X(sampler2DMSArray, GL_SAMPLER_2D_MULTISAMPLE_ARRAY)                       \
    X(samplerCubeShadow, GL_SAMPLER_CUBE_SHADOW)                               \
    X(samplerBuffer, GL_SAMPLER_BUFFER)                                        \
    X(sampler2DRect, GL_SAMPLER_2D_RECT)                                       \
    X(sampler2DRectShadow, GL_SAMPLER_2D_RECT_SHADOW)                          \
    X(isampler1D, GL_INT_SAMPLER_1D)                                           \
    X(isampler2D, GL_INT_SAMPLER_2D)                                           \
    X(isampler3D, GL_INT_SAMPLER_3D)                                           \
    X(isamplerCube, GL_INT_SAMPLER_CUBE)                                       \
    X(isampler1DArray, GL_INT_SAMPLER_1D_ARRAY)                                \
    X(isampler2DArray, GL_INT_SAMPLER_2D_ARRAY)                                \
    X(isampler2DMS, GL_INT_SAMPLER_2D_MULTISAMPLE)                             \
    X(isampler2DMSArray, GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY)                  \
    X(isamplerBuffer, GL_INT_SAMPLER_BUFFER)                                   \
    X(isampler2DRect, GL_INT_SAMPLER_2D_RECT)                                  \
    X(usampler1D, GL_UNSIGNED_INT_SAMPLER_1D)                                  \
    X(usampler2D, GL_UNSIGNED_INT_SAMPLER_2D)                                  \
    X(usampler3D, GL_UNSIGNED_INT_SAMPLER_3D)                                  \
    X(usamplerCube, GL_UNSIGNED_INT_SAMPLER_CUBE)                              \
    X(usampler1DArray, GL_UNSIGNED_INT_SAMPLER_1D_ARRAY)                       \
    X(usampler2DArray, GL_UNSIGNED_INT_SAMPLER_2D_ARRAY)                       \
    X(usampler2DMS, GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE)                    \
    X(usampler2DMSArray, GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY)         \
    X(usamplerBuffer, GL_UNSIGNED_INT_SAMPLER_BUFFER)                          \
    X(usampler2DRect, GL_UNSIGNED_INT_SAMPLER_2D_RECT)

#define HERA_GL_PIXEL_T(X)                                                     \
    X(none, GL_NONE)                                                           \
    X(i8, GL_BYTE)                                                             \
    X(u8, GL_UNSIGNED_BYTE)                                                    \
    X(i16, GL_SHORT)                                                           \
    X(u16, GL_UNSIGNED_SHORT)                                                  \
    X(i32, GL_INT)                                                             \
    X(u32, GL_UNSIGNED_INT)                                                    \
    X(f16, GL_HALF_FLOAT)                                                      \
    X(f32, GL_FLOAT)                                                           \
    X(u8_332, GL_UNSIGNED_BYTE_3_3_2)                                          \
    X(u8_233_r, GL_UNSIGNED_BYTE_2_3_3_REV)                                    \
    X(u16_565, GL_UNSIGNED_SHORT_5_6_5)                                        \
    X(u16_565_r, GL_UNSIGNED_SHORT_5_6_5_REV)                                  \
    X(u16_4444, GL_UNSIGNED_SHORT_4_4_4_4)                                     \
    X(u16_4444_r, GL_UNSIGNED_SHORT_4_4_4_4_REV)                               \
    X(u16_5551, GL_UNSIGNED_SHORT_5_5_5_1)                                     \
    X(u16_1555_r, GL_UNSIGNED_SHORT_1_5_5_5_REV)                               \
    X(u32_8888, GL_UNSIGNED_INT_8_8_8_8)                                       \
    X(u32_8888_r, GL_UNSIGNED_INT_8_8_8_8_REV)                                 \
    X(u32_10_10_10_2, GL_UNSIGNED_INT_10_10_10_2)                              \
    X(u32_2_10_10_10_r, GL_UNSIGNED_INT_2_10_10_10_REV)                        \
    X(u32_24_8, GL_UNSIGNED_INT_24_8)                                          \
    X(u32_10f11f11f_r, GL_UNSIGNED_INT_10F_11F_11F_REV)                        \
    X(u32_5999_r, GL_UNSIGNED_INT_5_9_9_9_REV)                                 \
    X(f32_u32_24_8_r, GL_FLOAT_32_UNSIGNED_INT_24_8_REV)

#define HERA_GL_PIXEL_F(X)                                                     \
    X(none, GL_NONE)                                                           \
    X(red, GL_RED)                                                             \
    X(blue, GL_BLUE)                                                           \
    X(green, GL_GREEN)                                                         \
    X(rg, GL_RG)                                                               \
    X(rgb, GL_RGB)                                                             \
    X(bgr, GL_BGR)                                                             \
    X(rgba, GL_RGBA)                                                           \
    X(bgra, GL_BGRA)                                                           \
    X(red_i, GL_RED_INTEGER)                                                   \
    X(blue_i, GL_BLUE_INTEGER)                                                 \
    X(green_i, GL_GREEN_INTEGER)                                               \
    X(rg_i, GL_RG_INTEGER)                                                     \
    X(rgb_i, GL_RGB_INTEGER)                                                   \
    X(bgr_i, GL_BGR_INTEGER)                                                   \
    X(rgba_i, GL_RGBA_INTEGER)                                                 \
    X(bgra_i, GL_BGRA_INTEGER)                                                 \
    X(stencil, GL_STENCIL_INDEX)                                               \
    X(depth, GL_DEPTH_COMPONENT)                                               \
    X(depth_stencil, GL_DEPTH_STENCIL)

#define HERA_GL_INTERNAL_F(X)                                                  \
    X(none, GL_NONE)                                                           \
    X(depth, GL_DEPTH_COMPONENT)                                               \
    X(depth_stencil, GL_DEPTH_STENCIL)                                         \
    X(red, GL_RED)                                                             \
    X(rg, GL_RG)                                                               \
    X(rgb, GL_RGB)                                                             \
    X(rgba, GL_RGBA)                                                           \
    X(r8, GL_R8)                                                               \
    X(r8s, GL_R8_SNORM)                                                        \
    X(r8i, GL_R8I)                                                             \
    X(r8ui, GL_R8UI)                                                           \
    X(r16, GL_R16)                                                             \
    X(r16s, GL_R16_SNORM)                                                      \
    X(r16i, GL_R16I)                                                           \
    X(r16ui, GL_R16UI)                                                         \
    X(r16f, GL_R16F)                                                           \
    X(r32i, GL_R32I)                                                           \
    X(r32ui, GL_R32UI)                                                         \
    X(r32f, GL_R32F)                                                           \
    X(rg8, GL_RG8)                                                             \
    X(rg8s, GL_RG8_SNORM)                                                      \
    X(rg8i, GL_RG8I)                                                           \
    X(rg8ui, GL_RG8UI)                                                         \
    X(rg16, GL_RG16)                                                           \
    X(rg16s, GL_RG16_SNORM)                                                    \
    X(rg16i, GL_RG16I)                                                         \
    X(rg16ui, GL_RG16UI)                                                       \
    X(rg16f, GL_RG16F)                                                         \
    X(rg32i, GL_RG32I)                                                         \
    X(rg32ui, GL_RG32UI)                                                       \
    X(rg32f, GL_RG32F)                                                         \
    X(r3g3b2, GL_R3_G3_B2)                                                     \
    X(r11f_g11f_b10f, GL_R11F_G11F_B10F)                                       \
    X(rgb4, GL_RGB4)                                                           \
    X(rgb5, GL_RGB5)                                                           \
    X(rgb8, GL_RGB8)                                                           \
    X(rgb8s, GL_RGB8_SNORM)                                                    \
    X(rgb8i, GL_RGB8I)                                                         \
    X(rgb8ui, GL_RGB8UI)                                                       \
    X(srgb8, GL_SRGB8)                                                         \
    X(rgb9e5, GL_RGB9_E5)                                                      \
    X(rgb10, GL_RGB10)                                                         \
    X(rgb12, GL_RGB12)                                                         \
    X(rgb16, GL_RGB16)                                                         \
    X(rgb16s, GL_RGB16_SNORM)                                                  \
    X(rgb16i, GL_RGB16I)                                                       \
    X(rgb16ui, GL_RGB16UI)                                                     \
    X(rgb16f, GL_RGB16F)                                                       \
    X(rgb32i, GL_RGB32I)                                                       \
    X(rgb32ui, GL_RGB32UI)                                                     \
    X(rgb32f, GL_RGB32F)                                                       \
    X(rgba2, GL_RGBA2)                                                         \
    X(rgba4, GL_RGBA4)                                                         \
    X(rgb5a1, GL_RGB5_A1)                                                      \
    X(rgba8, GL_RGBA8)                                                         \
    X(rgba8s, GL_RGBA8_SNORM)                                                  \
    X(rgba8i, GL_RGBA8I)                                                       \
    X(rgba8ui, GL_RGBA8UI)                                                     \
    X(srgb8a8, GL_SRGB8_ALPHA8)                                                \
    X(rgb10a2, GL_RGB10_A2)                                                    \
    X(rgb10a2ui, GL_RGB10_A2UI)                                                \
    X(rgba12, GL_RGBA12)                                                       \
    X(rgba16, GL_RGBA16)                                                       \
    X(rgba16s, GL_RGBA16_SNORM)                                                \
    X(rgba16i, GL_RGBA16I)                                                     \
    X(rgba16ui, GL_RGBA16UI)                                                   \
    X(rgba16f, GL_RGBA16F)                                                     \
    X(rgba32i, GL_RGBA32I)                                                     \
    X(rgba32ui, GL_RGBA32UI)                                                   \
    X(rgba32f, GL_RGBA32F)                                                     \
    X(d16, GL_DEPTH_COMPONENT16)                                               \
    X(d24, GL_DEPTH_COMPONENT24)                                               \
    X(d32, GL_DEPTH_COMPONENT32)                                               \
    X(d32f, GL_DEPTH_COMPONENT32F)                                             \
    X(d24s8, GL_DEPTH24_STENCIL8)                                              \
    X(d32fs8, GL_DEPTH32F_STENCIL8)                                            \
    X(stencil1, GL_STENCIL_INDEX1)                                             \
    X(stencil4, GL_STENCIL_INDEX4)                                             \
    X(stencil8, GL_STENCIL_INDEX8)                                             \
    X(stencil16, GL_STENCIL_INDEX16)

#define HERA_GL_BUFFER_T(X)                                                    \
    X(array, GL_ARRAY_BUFFER)                                                  \
    X(copy_read, GL_COPY_READ_BUFFER)                                          \
    X(copy_write, GL_COPY_WRITE_BUFFER)                                        \
    X(element_array, GL_ELEMENT_ARRAY_BUFFER)                                  \
    X(pixel_pack, GL_PIXEL_PACK_BUFFER)                                        \
    X(pixel_unpack, GL_PIXEL_UNPACK_BUFFER)                                    \
    X(texture, GL_TEXTURE_BUFFER)                                              \
    X(transform_feedback, GL_TRANSFORM_FEEDBACK_BUFFER)                        \
    X(uniform, GL_UNIFORM_BUFFER)

#define HERA_GL_BUFFER_USE(X)                                                  \
    X(static_draw, GL_STATIC_DRAW)                                             \
    X(static_read, GL_STATIC_READ)                                             \
    X(static_copy, GL_STATIC_COPY)                                             \
    X(stream_draw, GL_STREAM_DRAW)                                             \
    X(stream_read, GL_STREAM_READ)                                             \
    X(stream_copy, GL_STREAM_COPY)                                             \
    X(dynamic_draw, GL_DYNAMIC_DRAW)                                           \
    X(dynamic_read, GL_DYNAMIC_READ)                                           \
    X(dynamic_copy, GL_DYNAMIC_COPY)

#define HERA_GL_BUFFER_ACCESS(X)                                               \
    X(read_only, GL_READ_ONLY)                                                 \
    X(write_only, GL_WRITE_ONLY)                                               \
    X(read_write, GL_READ_WRITE)

#define HERA_GL_MAP_FLAG(X)                                                    \
    X(read, GL_MAP_READ_BIT)                                                   \
    X(write, GL_MAP_WRITE_BIT)                                                 \
    X(invalidate_range, GL_MAP_INVALIDATE_RANGE_BIT)                           \
    X(invalidate_buffer, GL_MAP_INVALIDATE_BUFFER_BIT)                         \
    X(flush_explicit, GL_MAP_FLUSH_EXPLICIT_BIT)                               \
    X(unsync, GL_MAP_UNSYNCHRONIZED_BIT)

#define HERA_GL_TEXTURE_T(X)                                                   \
    X(oneD, GL_TEXTURE_1D)                                                     \
    X(twoD, GL_TEXTURE_2D)                                                     \
    X(threeD, GL_TEXTURE_3D)                                                   \
    X(array_1d, GL_TEXTURE_1D_ARRAY)                                           \
    X(array_2d, GL_TEXTURE_2D_ARRAY)                                           \
    X(rectangle, GL_TEXTURE_RECTANGLE)                                         \
    X(cube_map, GL_TEXTURE_CUBE_MAP)                                           \
    X(cube_map_px, GL_TEXTURE_CUBE_MAP_POSITIVE_X)                             \
    X(cube_map_py, GL_TEXTURE_CUBE_MAP_POSITIVE_Y)                             \
    X(cube_map_pz, GL_TEXTURE_CUBE_MAP_POSITIVE_Z)                             \
    X(cube_map_nx, GL_TEXTURE_CUBE_MAP_NEGATIVE_X)                             \
    X(cube_map_ny, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y)                             \
    X(cube_map_nz, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)                             \
    X(buffer, GL_TEXTURE_BUFFER)                                               \
    X(multisample_2d, GL_TEXTURE_2D_MULTISAMPLE)                               \
    X(multisample_2d_array, GL_TEXTURE_2D_MULTISAMPLE_ARRAY)

#define HERA_GL_BIND_QUERY(X)                                                  \
    X(texture_1d, GL_TEXTURE_BINDING_1D)                                       \
    X(texture_2d, GL_TEXTURE_BINDING_2D)                                       \
    X(texture_3d, GL_TEXTURE_BINDING_3D)                                       \
    X(texture_cube_map, GL_TEXTURE_BINDING_CUBE_MAP)                           \
    X(texture_1d_array, GL_TEXTURE_BINDING_1D_ARRAY)                           \
    X(texture_2d_array, GL_TEXTURE_BINDING_2D_ARRAY)                           \
    X(texture_2d_multisample, GL_TEXTURE_BINDING_2D_MULTISAMPLE)               \
    X(texture_2d_multisample_array, GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY)   \
    X(texture_rectangle, GL_TEXTURE_BINDING_RECTANGLE)                         \
    X(texture_buffer, GL_TEXTURE_BINDING_BUFFER)                               \
    X(array_buffer, GL_ARRAY_BUFFER_BINDING)                                   \
    X(element_array_buffer, GL_ELEMENT_ARRAY_BUFFER_BINDING)                   \
    X(pixel_pack_buffer, GL_PIXEL_PACK_BUFFER_BINDING)                         \
    X(pixel_unpack_buffer, GL_PIXEL_UNPACK_BUFFER_BINDING)                     \
    X(transform_feedback_buffer, GL_TRANSFORM_FEEDBACK_BUFFER_BINDING)         \
    X(uniform_buffer, GL_UNIFORM_BUFFER_BINDING)                               \
    X(read_framebuffer, GL_READ_FRAMEBUFFER_BINDING)                           \
    X(draw_framebuffer, GL_DRAW_FRAMEBUFFER_BINDING)                           \
    X(renderbuffer, GL_RENDERBUFFER_BINDING)                                   \
    X(vertex_attrib_array_buffer, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING)       \
    X(vertex_array, GL_VERTEX_ARRAY_BINDING)                                   \
    X(active_texture, GL_ACTIVE_TEXTURE)                                       \
    X(pipeline, GL_PROGRAM_PIPELINE_BINDING)

#define HERA_GL_SHADER_T(X)                                                    \
    X(vertex, GL_VERTEX_SHADER)                                                \
    X(fragment, GL_FRAGMENT_SHADER)                                            \
    X(geometry, GL_GEOMETRY_SHADER)

#define HERA_GL_FRAMEBUFFER_T(X)                                               \
    X(read, GL_READ_FRAMEBUFFER)                                               \
    X(draw, GL_DRAW_FRAMEBUFFER)

#define HERA_GL_ERROR_T(X)                                                     \
    X(invalid_value, GL_INVALID_VALUE)                                         \
    X(invalid_enum, GL_INVALID_ENUM)                                           \
    X(invalid_op, GL_INVALID_OPERATION)                                        \
    X(oom, GL_OUT_OF_MEMORY)                                                   \
    X(invalid_fb_op, GL_INVALID_FRAMEBUFFER_OPERATION)                         \
    X(no_error, GL_NO_ERROR)

#define HERA_GL_PRIMITIVE_T(X)                                                 \
    X(points, GL_POINTS)                                                       \
    X(line_strip, GL_LINE_STRIP)                                               \
    X(line_loop, GL_LINE_LOOP)                                                 \
    X(lines, GL_LINES)                                                         \
    X(line_strip_adj, GL_LINE_STRIP_ADJACENCY)                                 \
    X(lines_adj, GL_LINES_ADJACENCY)                                           \
    X(triangle_strip, GL_TRIANGLE_STRIP)                                       \
    X(triangle_fan, GL_TRIANGLE_FAN)                                           \
    X(triangles, GL_TRIANGLES)                                                 \
    X(triangle_strip_adj, GL_TRIANGLE_STRIP_ADJACENCY)                         \
    X(triangles_adj, GL_TRIANGLES_ADJACENCY)                                   \
    X(patches, GL_PATCHES)

#define HERA_GL_ENUMS(X)                                                       \
    X(buffer_t, HERA_GL_BUFFER_T)                                              \
    X(buffer_use, HERA_GL_BUFFER_USE)                                          \
    X(buffer_access, HERA_GL_BUFFER_ACCESS)                                    \
    X(texture_t, HERA_GL_TEXTURE_T)                                            \
    X(bind_query, HERA_GL_BIND_QUERY)                                          \
    X(framebuffer_t, HERA_GL_FRAMEBUFFER_T)                                    \
    X(error_t, HERA_GL_ERROR_T)                                                \
    X(primitive_t, HERA_GL_PRIMITIVE_T)

#define HERA_GL_ALL(X)                                                         \
    X(gl_t, HERA_GL_T)                                                         \
    X(glsl_t, HERA_GLSL_T)                                                     \
    X(pixel_t, HERA_GL_PIXEL_T)                                                \
    X(pixel_f, HERA_GL_PIXEL_F)                                                \
    X(internal_f, HERA_GL_INTERNAL_F)                                          \
    X(shader_t, HERA_GL_SHADER_T)                                              \
    X(buffer_t, HERA_GL_BUFFER_T)                                              \
    X(buffer_use, HERA_GL_BUFFER_USE)                                          \
    X(buffer_access, HERA_GL_BUFFER_ACCESS)                                    \
    X(texture_t, HERA_GL_TEXTURE_T)                                            \
    X(bind_query, HERA_GL_BIND_QUERY)                                          \
    X(framebuffer_t, HERA_GL_FRAMEBUFFER_T)                                    \
    X(error_t, HERA_GL_ERROR_T)                                                \
    X(map_flag, HERA_GL_MAP_FLAG)                                              \
    X(primitive_t, HERA_GL_PRIMITIVE_T)

#define HERA_MAKE_ENUM(K, V) K = V,
#define HERA_MAKE_STRVALS(K, V) #K,

#define HERA_GL_ALL_BITS_IMPL(K, V) V |
#define HERA_GL_ALL_BITS(TYENUM) TYENUM(HERA_GL_ALL_BITS_IMPL) 0

#define HERA_MAKE_BITENUM(TYENUM)                                              \
    TYENUM(HERA_MAKE_ENUM)                                                     \
    all = HERA_GL_ALL_BITS(TYENUM)

#define HERA_MAKE_VCASE(K, V) case V:
#define HERA_MAKE_KCASE(K, V) case K:

#define HERA_MAKE_STR_VCASE(K, V)                                              \
    case V:                                                                    \
        return #K;

#define HERA_MAKE_STR_KCASE(K, V)                                              \
    case K:                                                                    \
        return #K;

#define HERA_MAKE_VALID_V(TYENUM)                                              \
    switch (value) {                                                           \
        TYENUM(HERA_MAKE_VCASE)                                                \
        return true;                                                           \
    default:                                                                   \
        return false;                                                          \
    }

#define HERA_MAKE_VALIDS_K_IMPL(TYNAME, TYENUM)                                \
    if constexpr (same_as<T, TYNAME>) {                                        \
        switch (value) {                                                       \
            using enum TYNAME;                                                 \
            TYENUM(HERA_MAKE_KCASE)                                            \
            return true;                                                       \
        default:                                                               \
            return false;                                                      \
        }                                                                      \
    }                                                                          \
    else

#define HERA_MAKE_VALIDS_K(ENUMS)                                              \
    ENUMS(HERA_MAKE_VALIDS_K_IMPL)                                             \
    {                                                                          \
        return false;                                                          \
    }

#define HERA_MAKE_STR(TYENUM)                                                  \
    switch (value) {                                                           \
        TYENUM(HERA_MAKE_STR_VCASE)                                            \
    default:                                                                   \
        return "unknown";                                                      \
    }

#define HERA_MAKE_STRS_V_IMPL(TYNAME, TYENUM)                                  \
    switch (value) {                                                           \
        TYENUM(HERA_MAKE_STR_VCASE)                                            \
    default:                                                                   \
        return "unknown";                                                      \
    }

#define HERA_MAKE_STRS_V(ENUMS) ENUMS(HERA_MAKE_STRS_V_IMPL) return "unknown";

#define HERA_MAKE_STRS_K_IMPL(TYNAME, TYENUM)                                  \
    if constexpr (same_as<T, TYNAME>) {                                        \
        switch (value) {                                                       \
            using enum TYNAME;                                                 \
            TYENUM(HERA_MAKE_STR_KCASE)                                        \
        default:                                                               \
            return "unknown";                                                  \
        }                                                                      \
    }                                                                          \
    else

#define HERA_MAKE_STRS_K(ENUMS)                                                \
    ENUMS(HERA_MAKE_STRS_K_IMPL)                                               \
    {                                                                          \
        return "unknown";                                                      \
    }

} // namespace

template<typename T>
    requires std::is_scoped_enum_v<T>
constexpr auto operator+(const T& v)
{
    return std::to_underlying(v);
}

// gl constants made more type-safe
template<typename T, typename Storage = GLenum>
struct gl_enum_t {
    Storage value;

private:
    constexpr gl_enum_t() : value{0} {}

public:
    template<typename U>
        requires std::convertible_to<U, Storage>
    constexpr gl_enum_t(U v) : value{static_cast<Storage>(v)}
    {
        if constexpr (HERA_DEBUG) {
            if (!static_cast<T*>(this)->valid()) {
                throw gl_error(
                    fmt::format("invalid value for {}", type_of<T>()));
            }
        }
    };

    template<typename U>
        requires same_as<U, typename T::value_type>
    constexpr gl_enum_t(U v) : value{static_cast<Storage>(v)} {};

    template<typename U>
        requires(requires {
            typename T::typespec;
            typename U::typespec;
        })
    explicit constexpr gl_enum_t(U v) : value{v.value}
    {
        if constexpr (HERA_DEBUG) {
            if (!static_cast<T*>(this)->valid()) {
                throw gl_error(fmt::format("invalid cast from {} to {}",
                                           type_of<U>(), type_of<T>()));
            }
        }
    };

    friend constexpr auto operator<=>(const gl_enum_t&,
                                      const gl_enum_t&) = default;
    template<typename U>
        requires same_as<U, typename T::value_type>
    constexpr bool operator==(const U& v) const
    {
        return static_cast<typename T::value_type>(value) == v;
    }

    constexpr operator auto() const { return typename T::value_type{value}; }
    explicit constexpr operator bool() const { return value != 0; }

    explicit constexpr operator Storage() const { return value; }
    constexpr Storage operator+() const { return value; }
    friend T;
};

// specifies any concrete opengl type
struct gl_t : gl_enum_t<gl_t> {
    enum class value_type : GLenum { HERA_GL_T(HERA_MAKE_ENUM) };
    using enum value_type;
    using gl_enum_t::gl_enum_t;
    using typespec = void;
    constexpr bool valid() const { HERA_MAKE_VALID_V(HERA_GL_T) }
    constexpr string_view str() const { HERA_MAKE_STR(HERA_GL_T) }
};

// specifies any glsl type.
struct glsl_t : gl_enum_t<glsl_t> {
    enum value_type : GLenum { HERA_GLSL_T(HERA_MAKE_ENUM) };
    using enum value_type;
    using gl_enum_t::gl_enum_t;
    using typespec = void;
    constexpr bool valid() const { HERA_MAKE_VALID_V(HERA_GLSL_T) }
    constexpr string_view str() const { HERA_MAKE_STR(HERA_GLSL_T) }
};

// specifies the data type of pixels.
struct pixel_t : gl_enum_t<pixel_t> {
    enum class value_type : GLenum { HERA_GL_PIXEL_T(HERA_MAKE_ENUM) };
    using enum value_type;
    using gl_enum_t::gl_enum_t;
    using typespec = void;
    constexpr bool valid() const { HERA_MAKE_VALID_V(HERA_GL_PIXEL_T) }
    constexpr string_view str() const { HERA_MAKE_STR(HERA_GL_PIXEL_T) }
};

// specifies the internal format of a texture.
struct internal_f : gl_enum_t<internal_f, GLint> {
    enum class value_type : GLint { HERA_GL_INTERNAL_F(HERA_MAKE_ENUM) };
    using enum value_type;
    using gl_enum_t::gl_enum_t;
    using typespec = void;
    constexpr bool valid() const { HERA_MAKE_VALID_V(HERA_GL_INTERNAL_F) }
    constexpr string_view str() const { HERA_MAKE_STR(HERA_GL_INTERNAL_F) }
};

// specifies the format of pixels.
struct pixel_f : gl_enum_t<pixel_f> {
    enum class value_type : GLenum { HERA_GL_PIXEL_F(HERA_MAKE_ENUM) };
    using enum value_type;
    using gl_enum_t::gl_enum_t;
    using typespec = void;
    constexpr pixel_f(internal_f v) : gl_enum_t{v.value} {}
    constexpr bool valid() const { HERA_MAKE_VALID_V(HERA_GL_PIXEL_F) }
    constexpr string_view str() const { HERA_MAKE_STR(HERA_GL_PIXEL_F) }
};

// specifies a shader type.
struct shader_t : gl_enum_t<shader_t> {
    enum class value_type : GLenum { HERA_GL_SHADER_T(HERA_MAKE_ENUM) };
    using enum value_type;
    using gl_enum_t::gl_enum_t;
    constexpr bool valid() const { HERA_MAKE_VALID_V(HERA_GL_SHADER_T) }
    constexpr string_view str() const { HERA_MAKE_STR(HERA_GL_SHADER_T) }

    constexpr GLbitfield bit() const
    {
        switch (value) {
        case GL_VERTEX_SHADER:
            return GL_VERTEX_SHADER_BIT;
        case GL_FRAGMENT_SHADER:
            return GL_FRAGMENT_SHADER_BIT;
        default:
            return 0;
        }
    }
};

// specifies a texture unit.
struct texture_u : gl_enum_t<texture_u, GLint> {
    enum class value_type : GLint {};

    template<typename U>
        requires std::convertible_to<U, GLint>
    constexpr texture_u(U v) : gl_enum_t{make_valid(v)} {};

    constexpr GLenum offset() const { return GL_TEXTURE0 + value; }

    constexpr string_view str() const
    {
        auto& val = strnames[value];
        return val.data();
    }
    constexpr bool valid() const { return value < 0xff; }

private:
    static constexpr auto strnames = [] consteval {
        constexpr auto maxunits = 0xff;
        constexpr auto bufsz = sizeof("texture unit 000");
        array<array<char, bufsz>, maxunits> buf{};
        int n = 0;
        for (auto& arr : buf) {
            auto loc = ranges::copy("texture unit ", arr.begin()).out;
            std::to_chars(loc, arr.end(), n);
            ++n;
        }
        return buf;
    }();

    template<typename U>
        requires std::convertible_to<U, GLint>
    static constexpr GLenum make_valid(U v)
    {
        if (v >= GL_TEXTURE0) {
            return v - GL_TEXTURE0;
        }
        else {
            return v;
        }
    }
};

enum class buffer_t : GLenum { HERA_GL_BUFFER_T(HERA_MAKE_ENUM) };
enum class buffer_use : GLenum { HERA_GL_BUFFER_USE(HERA_MAKE_ENUM) };
enum class buffer_access : GLenum { HERA_GL_BUFFER_ACCESS(HERA_MAKE_ENUM) };
enum class texture_t : GLenum { HERA_GL_TEXTURE_T(HERA_MAKE_ENUM) };
enum class bind_query : GLenum { HERA_GL_BIND_QUERY(HERA_MAKE_ENUM) };
enum class framebuffer_t : GLenum { HERA_GL_FRAMEBUFFER_T(HERA_MAKE_ENUM) };
enum class error_t : GLenum { HERA_GL_ERROR_T(HERA_MAKE_ENUM) };
enum class primitive_t : GLenum { HERA_GL_PRIMITIVE_T(HERA_MAKE_ENUM) };

template<typename T>
struct gl_bitfield_t {
    GLbitfield value;

private:
    constexpr gl_bitfield_t() : value{0} {}

public:
    template<typename U>
        requires std::convertible_to<U, GLbitfield>
    constexpr gl_bitfield_t(U v) : value{static_cast<GLbitfield>(v)}
    {
        if constexpr (HERA_DEBUG) {
            if (!static_cast<T*>(this)->valid()) {
                throw gl_error(
                    fmt::format("invalid value for {}", type_of<T>()));
            }
        }
    };

    template<typename U>
        requires same_as<U, typename T::value_type>
    constexpr gl_bitfield_t(U v) : value{static_cast<GLbitfield>(v)} {};

    template<typename U>
        requires convertible_to<U, T> || same_as<U, typename T::value_type>
    friend constexpr T operator|(const gl_bitfield_t& l, const U& r)
    {
        return l.value | static_cast<GLbitfield>(r);
    }
    template<typename U>
        requires convertible_to<U, T> || same_as<U, typename T::value_type>
    friend constexpr T operator&(const gl_bitfield_t& l, const U& r)
    {
        return l.value & static_cast<GLbitfield>(r);
    }
    template<typename U>
        requires convertible_to<U, T> || same_as<U, typename T::value_type>
    friend constexpr T operator^(const gl_bitfield_t& l, const U& r)
    {
        return l.value ^ static_cast<GLbitfield>(r);
    }

    friend constexpr T& operator|=(gl_bitfield_t& l, const auto& r)
    {
        l.value = l.value | static_cast<GLenum>(r);
        return static_cast<T&>(l);
    }
    friend constexpr T& operator&=(gl_bitfield_t& l, const auto& r)
    {
        l.value = l.value & static_cast<GLenum>(r);
        return static_cast<T&>(l);
    }
    friend constexpr T& operator^=(gl_bitfield_t& l, const auto& r)
    {
        l.value = l.value ^ static_cast<GLenum>(r);
        return static_cast<T&>(l);
    }

    friend constexpr auto operator<=>(const gl_bitfield_t&,
                                      const gl_bitfield_t&) = default;

    constexpr operator auto() const { return typename T::value_type{value}; }
    explicit constexpr operator bool() const { return value != 0; }

    explicit constexpr operator GLbitfield() const { return value; }
    constexpr GLbitfield operator+() const { return value; }
    friend T;
};

struct map_flag : gl_bitfield_t<map_flag> {
    enum class value_type : GLbitfield { HERA_MAKE_BITENUM(HERA_GL_MAP_FLAG) };
    using enum value_type;
    using gl_bitfield_t::gl_bitfield_t;

    constexpr bool valid() const
    {
        constexpr auto nonef = ~(std::to_underlying(all));
        return !(value & nonef);
    }

    constexpr string_view str() const
    {
        if (valid()) {
            return getstr(value);
        }
        else {
            return "unknown";
        }
    }

private:
    static constexpr array<string_view, std::popcount(+all)> strnames = {
        HERA_GL_MAP_FLAG(HERA_MAKE_STRVALS)};

    static constexpr size_t bytes_for(GLbitfield val)
    {
        if (val == 0)
            return 0;
        auto bytes = std::popcount(val) - 1;
        for (auto i = 1u; i < +all; i <<= 1) {
            if (val & i)
                bytes += strnames[std::countr_zero(i)].size();
        }
        return bytes;
    }

    template<GLbitfield val>
    static constexpr array<char, bytes_for(val)> str_for()
    {
        array<char, bytes_for(val)> buf{};
        auto loc = buf.begin();
        for (auto i = 1u; i < +all; i <<= 1) {
            auto idx = std::countr_zero(i);
            if (val & i) {
                loc = ranges::copy(strnames[idx], loc).out;
                if (i < std::bit_floor(val)) {
                    *loc++ = '+';
                }
            }
        }
        return buf;
    }

    static constexpr size_t total_bytes = [] consteval {
        auto nstrs = std::popcount(+all);
        auto neach = (1 << nstrs) / 2;
        auto bytes = 0;
        for (auto& s : strnames) {
            bytes += neach * s.size();
        }
        for (int i = 2; i <= nstrs; ++i) {
            bytes += (i - 1) * binom(nstrs, i);
        }
        return bytes;
    }();

    template<GLbitfield Z, GLbitfield... I>
    static consteval array<char, total_bytes>
    mkstrs(std::integer_sequence<GLbitfield, Z, I...>)
    {
        array<char, total_bytes> buf{};

        constexpr auto writeinto = [](char*& loc, const auto& arr) consteval {
            loc = ranges::copy(arr, loc).out;
        };

        auto loc = buf.begin();
        (writeinto(loc, str_for<I>()), ...);

        return buf;
    }

    static constexpr size_t offset_for(GLbitfield n)
    {
        size_t off = 0;
        for (auto i = 0u; i < n; ++i) {
            off += bytes_for(i);
        }
        return off;
    }

    static constexpr string_view getstr(GLbitfield n)
    {
        constexpr auto ncombs = (1 << std::popcount(+all));
        static constexpr auto allstrs =
            mkstrs(std::make_integer_sequence<GLbitfield, ncombs>());
        auto nb = bytes_for(n);
        auto off = offset_for(n);
        auto ptr = allstrs.data();
        return string_view{ptr + off, nb};
    }
};

template<typename T>
concept gl_enum =
    std::derived_from<T, gl_enum_t<T>> || same_as<T, buffer_t> ||
    same_as<T, buffer_use> || same_as<T, buffer_access> ||
    same_as<T, texture_t> || same_as<T, bind_query> ||
    same_as<T, framebuffer_t> || same_as<T, error_t> || same_as<T, primitive_t>;

// specifies that a type is used to specify an OpenGL type.
template<typename T>
concept gl_typespec = gl_enum<T> && requires { typename T::typespec; };

// specifies that a type is a GLbitfield value.
template<typename T>
concept gl_bitfield = std::derived_from<T, gl_bitfield_t<T>>;

template<typename T>
    requires gl_enum<T> || gl_bitfield<T>
constexpr string_view gl_str(const T& value)
{
    if constexpr (std::derived_from<T, gl_enum_t<T>> ||
                  std::derived_from<T, gl_bitfield_t<T>>) {
        return value.str();
    }
    else
        HERA_MAKE_STRS_K(HERA_GL_ENUMS)
}

constexpr string_view gl_str(GLenum value)
{
    HERA_MAKE_STRS_V(HERA_GL_ALL)
}

template<typename T>
    requires gl_enum<T> || gl_bitfield<T>
constexpr bool gl_valid(const T& value)
{
    if constexpr (std::derived_from<T, gl_enum_t<T>> ||
                  std::derived_from<T, gl_bitfield_t<T>>) {
        return value.valid();
    }
    else
        HERA_MAKE_VALIDS_K(HERA_GL_ENUMS)
}

// logs any GL errors and returns the last error code in the error log.
inline void
checkerror(std::source_location loc = std::source_location::current())
{
    if constexpr (HERA_DEBUG) {
        error_t err;
        auto func = loc.function_name();
        auto file = loc.file_name();
        auto line = loc.line();
        while ((err = error_t{glGetError()}) != error_t::no_error) {
            LOG_ERROR("GL:{}:{}:{}:{}", gl_str(err), func, file, line);
            /*
             * throw gl_error{
             *     fmt::format("GL:{}:{}:{}:{}", gl_str(err), func, file,
             * line)};
             */
        }
    }
}

// specifies that a type is an OpenGL primitive type capable of being used as a
// vertex attribute.
template<typename T>
concept gl_type = std::is_arithmetic_v<T> && sizeof(T) <= 4;

// specifies that a type is appropriate for OpenGL element array buffers.
template<typename T>
concept gl_index_type =
    std::unsigned_integral<T> && sizeof(T) <= 4 && !same_as<T, bool>;

// specifies a scalar OpenGL uniform type.
template<typename T>
concept gl_scalar = std::is_arithmetic_v<T> && sizeof(T) == 4;

// the number of scalar elements in a type.
template<typename T>
    requires gl_scalar<T> || gl_scalar<element_t<T>>
consteval size_t gl_length(const T& t)
{
    return hera::lengthof(t);
}

// OpenGL vector type. i.e. glm::vecN and array<gl_scalar, N> where N=2,3,4.
template<typename T, size_t N = gl_length(T{}), typename V = element_t<T>>
concept gl_vector = gl_scalar<element_t<T>> && gl_length(T{}) <= 4 &&
                    gl_length(T{}) > 1 && gl_length(T{}) == N && !requires {
                        typename T::col_type;
                    } && same_as<element_t<T>, V>;

// OpenGL matrix type.
template<typename T, typename V = T::value_type>
concept gl_matrix =
    gl_vector<typename T::row_type> && gl_vector<typename T::col_type> &&
    same_as<typename T::value_type, V>;

template<typename T, size_t C, size_t R = C, typename V = T::value_type>
concept gl_matrixN = gl_matrix<T, V> && gl_cols(T{}) == C && gl_rows(T{}) == R;

// any type able to be used as a single uniform variable.
template<typename T>
concept glsl_type = gl_scalar<T> || gl_vector<T> || gl_matrix<T>;

/*
template<typename T>
concept glsl_array =
    ranges::contiguous_range<T> && glsl_type<ranges::range_value_t<T>>;
    */

template<typename R>
concept glsl_array = spanner<R> && glsl_type<range_v<R>>;

template<typename T>
concept glm_type = requires(const T& v) { glm::value_ptr(v); };

template<typename T>
struct uniform_traits;

template<typename T>
    requires glsl_type<T>
struct uniform_traits<T> {
    using value_type = decay_t<T>;
    using storage_type = element_t<T>;

    // Uniform variables other than arrays have size 1.
    static constexpr GLint size(const T&) { return 1; }
    static constexpr const storage_type* storage(const T& v)
    {
        if constexpr (gl_scalar<value_type>) {
            return &v;
        }
        else if constexpr (glm_type<value_type>) {
            return glm::value_ptr(v);
        }
        else {
            static_assert(false, "bad type");
        }
    }
};

template<typename T>
    requires glsl_array<T>
struct uniform_traits<T> {
    using value_type = ranges::range_value_t<T>;
    using storage_type = element_t<value_type>;

    // Size of a uniform variable. i.e. the number of uniforms in a variable.
    static constexpr GLint size(const T& v) { return ranges::ssize(v); }

    static constexpr const storage_type* storage(const T& v)
    {
        auto ptr = ranges::cbegin(v);
        if constexpr (glm_type<value_type>) {
            return glm::value_ptr(*ptr);
        }
        else {
            return ptr;
        }
    }
};

template<>
struct uniform_traits<texture_u> {
    using value_type = GLint;
    using storage_type = GLint;

    static constexpr GLint size(const texture_u&) { return 1; }
    static constexpr const GLint* storage(const texture_u& v)
    {
        return &v.value;
    }
};

// any type able to be used as a uniform variable.
template<typename T>
concept uniformable = requires(const T& v) {
    typename uniform_traits<T>::value_type;
    typename uniform_traits<T>::storage_type;
    { uniform_traits<T>::size(v) } -> same_as<GLint>;
    {
        uniform_traits<T>::storage(v)
    } -> same_as<const typename uniform_traits<T>::storage_type*>;
};

template<typename T>
constexpr GLint uniform_size(const T& v)
{
    return uniform_traits<T>::size(v);
}

// Returns a pointer to the storage of a uniformable variable.
template<typename T>
constexpr auto uniform_storage(const T& v)
{
    return uniform_traits<T>::storage(v);
}

// the std140 alignment requirement of a type.
template<uniformable T>
consteval size_t gl_alignof()
{
    constexpr size_t N = 4;
    if constexpr (glsl_type<T>) {
        constexpr size_t len = gl_length(T{});
        if constexpr (len == 1)
            return N;
        else if constexpr (len == 2)
            return 2 * N;
        else
            return 4 * N;
    }
    else {
        return 4 * N;
    }
}

template<uniformable T>
constexpr size_t gl_alignof(const T&)
{
    return gl_alignof<T>();
}

// a wrapper type that aligns T to its GLSL std140 requirement.
template<glsl_type T>
struct alignas(gl_alignof(T{})) gl_aligned {
    T _data;
};

// returns the number of rows in a given matrix.
template<gl_matrix T>
consteval size_t gl_rows()
{
    return sizeof(typename T::col_type) / sizeof(typename T::value_type);
}

// returns the number of rows in a given matrix.
template<gl_matrix T>
constexpr size_t gl_rows(const T&)
{
    return gl_rows<T>();
}

// returns the number of columns in a given matrix.
template<gl_matrix T>
consteval size_t gl_cols()
{
    return sizeof(typename T::row_type) / sizeof(typename T::value_type);
}

// returns the number of columns in a given matrix.
template<gl_matrix T>
consteval size_t gl_cols(const T&)
{
    return gl_cols<T>();
}

// returns the number of bytes this uniform variable occupies in a shader.
template<glsl_type T>
consteval size_t gl_size()
{
    if constexpr (gl_matrix<T>) {
        return sizeof(vec4) * gl_cols(T{});
    }
    else if constexpr (gl_vector<T> || gl_scalar<T>) {
        return gl_alignof(T{});
    }
    else {
        throw hera::runtime_error("bad gl_size");
    }
}

// returns the number of bytes this uniform variable occupies in a shader.
template<glsl_type T>
consteval size_t gl_size(const T&)
{
    return gl_size<T>();
}

// returns the number of bytes this uniform variable array occupies in a
// shader
template<glsl_array T>
constexpr size_t gl_size(const T& v)
{
    return uniform_size(v) * gl_size(vec4{});
}

// returns the GL enum constant for the given type.
template<typename T>
    requires gl_type<T> || glsl_type<T>
consteval GLenum gl_typeof()
{
    if constexpr (same_as<T, int8_t>) {
        return GL_BYTE;
    }
    else if constexpr (same_as<T, uint8_t>) {
        return GL_UNSIGNED_BYTE;
    }
    else if constexpr (same_as<T, int16_t>) {
        return GL_SHORT;
    }
    else if constexpr (same_as<T, uint16_t>) {
        return GL_UNSIGNED_SHORT;
    }
    else if constexpr (same_as<T, uint32_t>) {
        return GL_UNSIGNED_INT;
    }
    else if constexpr (gl_vector<T, 2, uint32_t>) {
        return GL_UNSIGNED_INT_VEC2;
    }
    else if constexpr (gl_vector<T, 3, uint32_t>) {
        return GL_UNSIGNED_INT_VEC3;
    }
    else if constexpr (gl_vector<T, 4, uint32_t>) {
        return GL_UNSIGNED_INT_VEC4;
    }
    else if constexpr (same_as<T, int32_t>) {
        return GL_INT;
    }
    else if constexpr (gl_vector<T, 2, int32_t>) {
        return GL_INT_VEC2;
    }
    else if constexpr (gl_vector<T, 3, int32_t>) {
        return GL_INT_VEC3;
    }
    else if constexpr (gl_vector<T, 4, int32_t>) {
        return GL_INT_VEC4;
    }
    else if constexpr (same_as<T, float>) {
        return GL_FLOAT;
    }
    else if constexpr (gl_vector<T, 2, float>) {
        return GL_FLOAT_VEC2;
    }
    else if constexpr (gl_vector<T, 3, float>) {
        return GL_FLOAT_VEC3;
    }
    else if constexpr (gl_vector<T, 4, float>) {
        return GL_FLOAT_VEC4;
    }
    else if constexpr (same_as<T, double>) {
        return GL_DOUBLE;
    }
    else if constexpr (gl_vector<T, 2, double>) {
        return GL_DOUBLE_VEC2;
    }
    else if constexpr (gl_vector<T, 3, double>) {
        return GL_DOUBLE_VEC3;
    }
    else if constexpr (gl_vector<T, 4, double>) {
        return GL_DOUBLE_VEC4;
    }
    else if constexpr (same_as<T, mat2>) {
        return GL_FLOAT_MAT2;
    }
    else if constexpr (same_as<T, mat2x3>) {
        return GL_FLOAT_MAT2x3;
    }
    else if constexpr (same_as<T, mat2x4>) {
        return GL_FLOAT_MAT2x4;
    }
    else if constexpr (same_as<T, mat3>) {
        return GL_FLOAT_MAT3;
    }
    else if constexpr (same_as<T, mat3x2>) {
        return GL_FLOAT_MAT3x2;
    }
    else if constexpr (same_as<T, mat3x4>) {
        return GL_FLOAT_MAT3x4;
    }
    else if constexpr (same_as<T, mat4>) {
        return GL_FLOAT_MAT4;
    }
    else if constexpr (same_as<T, mat4x2>) {
        return GL_FLOAT_MAT4x2;
    }
    else if constexpr (same_as<T, mat4x3>) {
        return GL_FLOAT_MAT4x3;
    }
    else if constexpr (same_as<T, glm::dmat2>) {
        return GL_DOUBLE_MAT2;
    }
    else if constexpr (same_as<T, glm::dmat2x3>) {
        return GL_DOUBLE_MAT2x3;
    }
    else if constexpr (same_as<T, glm::dmat2x4>) {
        return GL_DOUBLE_MAT2x4;
    }
    else if constexpr (same_as<T, glm::dmat3>) {
        return GL_DOUBLE_MAT3;
    }
    else if constexpr (same_as<T, glm::dmat3x2>) {
        return GL_DOUBLE_MAT3x2;
    }
    else if constexpr (same_as<T, glm::dmat3x4>) {
        return GL_DOUBLE_MAT3x4;
    }
    else if constexpr (same_as<T, glm::dmat4>) {
        return GL_DOUBLE_MAT4;
    }
    else if constexpr (same_as<T, glm::dmat4x2>) {
        return GL_DOUBLE_MAT4x2;
    }
    else if constexpr (same_as<T, glm::dmat4x3>) {
        return GL_DOUBLE_MAT4x3;
    }
    else {
        throw hera::runtime_error("bad type");
    }
}

// returns the GL enum constant for the given type.
template<typename T, typename R = GLenum>
    requires gl_type<T> || glsl_type<T>
constexpr R gl_typeof(const T&)
{
    return gl_typeof<T>();
}

// dimensionality of texture type
constexpr int gl_dimensions(texture_t tex)
{
    using enum texture_t;
    switch (tex) {
    case oneD:
        return 1;
    case twoD:
    case multisample_2d:
    case array_1d:
    case rectangle:
    case cube_map:
    case cube_map_px:
    case cube_map_py:
    case cube_map_pz:
    case cube_map_nx:
    case cube_map_ny:
    case cube_map_nz:
        return 2;
    case threeD:
    case array_2d:
    case multisample_2d_array:
        return 3;
    default:
        throw hera::runtime_error("bad texture_t");
    }
}

// number of components in a texture format.
constexpr size_t gl_components(GLenum format)
{
    switch (format) {
    case GL_RED:
    case GL_BLUE:
    case GL_GREEN:
    case GL_RED_INTEGER:
    case GL_BLUE_INTEGER:
    case GL_GREEN_INTEGER:
    case GL_DEPTH_COMPONENT:
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32:
    case GL_DEPTH_COMPONENT32F:
    case GL_STENCIL_INDEX:
    case GL_STENCIL_INDEX1:
    case GL_STENCIL_INDEX4:
    case GL_STENCIL_INDEX8:
    case GL_STENCIL_INDEX16:
    case GL_R8:
    case GL_R8_SNORM:
    case GL_R8I:
    case GL_R8UI:
    case GL_R16:
    case GL_R16_SNORM:
    case GL_R16I:
    case GL_R16UI:
    case GL_R16F:
    case GL_R32F:
    case GL_R32I:
    case GL_R32UI:
        return 1;
    case GL_DEPTH_STENCIL:
    case GL_DEPTH24_STENCIL8:
    case GL_DEPTH32F_STENCIL8:
    case GL_RG:
    case GL_RG_INTEGER:
    case GL_RG8:
    case GL_RG8_SNORM:
    case GL_RG8I:
    case GL_RG8UI:
    case GL_RG16:
    case GL_RG16_SNORM:
    case GL_RG16I:
    case GL_RG16UI:
    case GL_RG16F:
    case GL_RG32I:
    case GL_RG32UI:
    case GL_RG32F:
        return 2;
    case GL_RGB:
    case GL_BGR:
    case GL_BGR_INTEGER:
    case GL_RGB_INTEGER:
    case GL_R3_G3_B2:
    case GL_R11F_G11F_B10F:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB8_SNORM:
    case GL_RGB8I:
    case GL_RGB8UI:
    case GL_SRGB8:
    case GL_RGB9_E5:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
    case GL_RGB16_SNORM:
    case GL_RGB16I:
    case GL_RGB16UI:
    case GL_RGB16F:
    case GL_RGB32I:
    case GL_RGB32UI:
    case GL_RGB32F:
        return 3;
    case GL_BGRA:
    case GL_BGRA_INTEGER:
    case GL_RGBA_INTEGER:
    case GL_RGBA:
    case GL_RGBA2:
    case GL_RGBA4:
    case GL_RGB5_A1:
    case GL_RGBA8:
    case GL_RGBA8_SNORM:
    case GL_RGBA8I:
    case GL_RGBA8UI:
    case GL_SRGB8_ALPHA8:
    case GL_RGB10_A2:
    case GL_RGB10_A2UI:
    case GL_RGBA12:
    case GL_RGBA16:
    case GL_RGBA16_SNORM:
    case GL_RGBA16I:
    case GL_RGBA16UI:
    case GL_RGBA16F:
    case GL_RGBA32I:
    case GL_RGBA32UI:
    case GL_RGBA32F:
        return 4;
    default:
        throw hera::runtime_error("unrecognized format");
    }
}

// number of components in a texture format.
constexpr size_t gl_components(pixel_f pf)
{
    return gl_components(+pf.value);
}

// number of components in a texture format.
constexpr size_t gl_components(internal_f inf)
{
    return gl_components(+inf.value);
}

// given pixel data and format, returns the number of pixels.
template<spanner R>
    requires gl_type<range_v<R>>
constexpr size_t gl_npixels(const R& data, GLenum pixelformat)
{
    return ranges::size(data) / gl_components(pixelformat);
}

// size in bytes of given GL type enum.
constexpr size_t gl_sizeof(GLenum ty)
{
    switch (ty) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return 1uz;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        return 2uz;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
        return 4uz;
    case GL_FLOAT_VEC2:
    case GL_UNSIGNED_INT_VEC2:
    case GL_INT_VEC2:
        return 4uz * 2;
    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
    case GL_UNSIGNED_INT_VEC3:
        return 4uz * 3;
    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC4:
        return 4uz * 4;
    case GL_FLOAT_MAT2:
        return 2uz * 2 * 4;
    case GL_FLOAT_MAT3:
        return 3uz * 3 * 4;
    case GL_FLOAT_MAT4:
        return 4uz * 4 * 4;
    case GL_FLOAT_MAT2x3:
        return 2uz * 3 * 4;
    case GL_FLOAT_MAT2x4:
        return 2uz * 4 * 4;
    case GL_FLOAT_MAT3x2:
        return 3uz * 2 * 4;
    case GL_FLOAT_MAT3x4:
        return 3uz * 4 * 4;
    case GL_FLOAT_MAT4x2:
        return 4uz * 2 * 4;
    case GL_FLOAT_MAT4x3:
        return 4uz * 3 * 4;
        // just in case
    case GL_INVALID_VALUE:
    case GL_INVALID_ENUM:
    case GL_INVALID_INDEX:
    case GL_INVALID_OPERATION:
    default:
        throw hera::runtime_error("unrecognized type");
    }
}

template<typename T>
    requires same_as<T, bool> || std::integral<T> || std::floating_point<T> ||
             same_as<T, string_view> || gl_vector<T>
T get(GLenum pname)
{
    if constexpr (same_as<T, bool>) {
        GLboolean rv;
        glGetBooleanv(pname, &rv);
        return rv;
    }
    else if constexpr (std::integral<T> && sizeof(T) < sizeof(GLint64)) {
        GLint rv;
        glGetIntegerv(pname, &rv);
        return rv;
    }
    else if constexpr (std::integral<T> && sizeof(T) >= sizeof(GLint64)) {
        GLint64 rv;
        glGetInteger64v(pname, &rv);
        return rv;
    }
    else if constexpr (same_as<T, float>) {
        GLfloat rv;
        glGetFloatv(pname, &rv);
        return rv;
    }
    else if constexpr (same_as<T, double>) {
        GLdouble rv;
        glGetDoublev(pname, &rv);
        return rv;
    }
    else if constexpr (same_as<T, string_view>) {
        const GLubyte* rv = glGetString(pname);
        if (rv) {
            return reinterpret_cast<const char*>(rv);
        }
        else {
            throw gl_error("glGetString returned null");
        }
    }
    else if constexpr (gl_vector<T>) {
        using value_t = typename T::value_type;
        T rv;
        if constexpr (same_as<value_t, int>) {
            glGetIntegerv(pname, glm::value_ptr(rv));
        }
        else if constexpr (same_as<value_t, float>) {
            glGetFloatv(pname, glm::value_ptr(rv));
        }
        else if constexpr (same_as<value_t, double>) {
            glGetDoublev(pname, glm::value_ptr(rv));
        }
        else if constexpr (same_as<value_t, bool>) {
            glGetBooleanv(pname, glm::value_ptr(rv));
        }
        return rv;
    }
    else {
        assert(!"unexpected param type");
    }
}

template<typename T>
    requires same_as<T, bool> || std::integral<T> || std::floating_point<T> ||
             same_as<T, string_view> || gl_vector<T>
T get(GLenum pname, GLuint index)
{
    if constexpr (same_as<T, bool>) {
        GLboolean rv;
        glGetBooleani_v(pname, index, &rv);
        return rv;
    }
    else if constexpr (std::integral<T> && sizeof(T) < sizeof(GLint64)) {
        GLint rv;
        glGetIntegeri_v(pname, index, &rv);
        return rv;
    }
    else if constexpr (std::integral<T> && sizeof(T) >= sizeof(GLint64)) {
        GLint64 rv;
        glGetInteger64i_v(pname, index, &rv);
        return rv;
    }
    else if constexpr (same_as<T, float>) {
        GLfloat rv;
        glGetFloati_v(pname, index, &rv);
        return rv;
    }
    else if constexpr (same_as<T, double>) {
        GLdouble rv;
        glGetDoublei_v(pname, index, &rv);
        return rv;
    }
    else if constexpr (same_as<T, string_view>) {
        const GLubyte* rv = glGetStringi(pname, index);
        if (rv) {
            return reinterpret_cast<const char*>(rv);
        }
        else {
            throw gl_error("glGetString returned null");
        }
    }
    else if constexpr (gl_vector<T>) {
        using value_t = typename T::value_type;
        T rv;
        if constexpr (same_as<value_t, int>) {
            glGetIntegeri_v(pname, index, glm::value_ptr(rv));
        }
        else if constexpr (same_as<value_t, float>) {
            glGetFloati_v(pname, index, glm::value_ptr(rv));
        }
        else if constexpr (same_as<value_t, double>) {
            glGetDoublei_v(pname, index, glm::value_ptr(rv));
        }
        else if constexpr (same_as<value_t, bool>) {
            glGetBooleani_v(pname, index, glm::value_ptr(rv));
        }
        return rv;
    }
    else {
        assert(!"unexpected param type");
    }
}

} // namespace gl

template<>
struct enable_bitfield<gl::map_flag::value_type> {
    using value = void;
};

} // namespace hera

template<typename T>
    requires hera::gl::gl_enum<T> || hera::gl::gl_bitfield<T>
struct fmt::formatter<T> : formatter<string_view> {
    auto format(const T& val, auto& ctx)
    {
        return formatter<string_view>::format(hera::gl::gl_str(val), ctx);
    }
};

template<>
struct fmt::formatter<hera::gl::texture_u> : formatter<string_view> {
    auto format(const hera::gl::texture_u& val, auto& ctx)
    {

        return formatter<string_view>::format(fmt::format("unit{}", val), ctx);
    }
};

#undef HERA_MAKE_ENUM
#undef HERA_MAKE_STRVALS
#undef HERA_GL_ALL_BITS
#undef HERA_GL_ALL_BITS_IMPL
#undef HERA_MAKE_BITENUM
#undef HERA_MAKE_VCASE
#undef HERA_MAKE_KCASE
#undef HERA_MAKE_STR_VCASE
#undef HERA_MAKE_STR_KCASE
#undef HERA_MAKE_VALID_V
#undef HERA_MAKE_VALIDS_K_IMPL
#undef HERA_MAKE_VALIDS_K
#undef HERA_MAKE_STR
#undef HERA_MAKE_STRS_V_IMPL
#undef HERA_MAKE_STRS_V
#undef HERA_MAKE_STRS_K_IMPL
#undef HERA_MAKE_STRS_K
#undef HERA_GL_ENUMS
#undef HERA_GL_ALL
#undef HERA_GL_T
#undef HERA_GLSL_T
#undef HERA_GL_PIXEL_F
#undef HERA_GL_PIXEL_T
#undef HERA_GL_INTERNAL_F
#undef HERA_GL_SHADER_T
#undef HERA_GL_BUFFER_T
#undef HERA_GL_BUFFER_USE
#undef HERA_GL_BUFFER_ACCESS
#undef HERA_GL_TEXTURE_T
#undef HERA_GL_BIND_QUERY
#undef HERA_GL_FRAMEBUFFER_T
#undef HERA_GL_ERROR_T
#undef HERA_GL_MAP_FLAG
#undef HERA_GL_PRIMITIVE_T

#endif
// NOLINTEND(bugprone-macro-*,performance-enum-size)
