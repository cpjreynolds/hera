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

#ifndef HERA_GL_DETAIL_HPP
#define HERA_GL_DETAIL_HPP

#include <hera/common.hpp>
#include <hera/gl/common.hpp>

namespace hera::gl::detail {

template<typename T>
concept has_value_type = requires { typename T::value_type; };

template<typename T, typename Enable = void>
struct gl_attribute_element {};

template<typename T>
struct gl_attribute_element<T, std::enable_if_t<std::is_array_v<T>>> {
    using type = std::remove_extent_t<T>;
};

template<typename T>
struct gl_attribute_element<T, std::enable_if_t<has_value_type<T>>> {
    using type = T::value_type;
};

template<typename T>
struct gl_attribute_element<T, std::enable_if_t<std::is_arithmetic_v<T>>> {
    using type = T;
};

template<typename T>
using gl_attribute_element_t = gl_attribute_element<T>::type;

template<typename T, typename Enable = void>
struct gl_attribute_type {};

template<typename T>
inline constexpr GLenum gl_attribute_type_v = gl_attribute_type<T>::value;

template<typename T>
struct gl_attribute_type<T, std::enable_if_t<std::is_array_v<T>>>
    : std::integral_constant<
          GLenum, gl_attribute_type_v<std::remove_all_extents_t<T>>> {};

template<typename T>
struct gl_attribute_type<T, std::enable_if_t<has_value_type<T>>>
    : std::integral_constant<GLenum,
                             gl_attribute_type_v<typename T::value_type>> {};

template<>
struct gl_attribute_type<GLfloat> : std::integral_constant<GLenum, GL_FLOAT> {};

template<>
struct gl_attribute_type<GLbyte> : std::integral_constant<GLenum, GL_BYTE> {};

template<>
struct gl_attribute_type<GLubyte>
    : std::integral_constant<GLenum, GL_UNSIGNED_BYTE> {};

template<>
struct gl_attribute_type<GLshort> : std::integral_constant<GLenum, GL_SHORT> {};

template<>
struct gl_attribute_type<GLushort>
    : std::integral_constant<GLenum, GL_UNSIGNED_SHORT> {};

template<>
struct gl_attribute_type<GLint> : std::integral_constant<GLenum, GL_INT> {};

template<>
struct gl_attribute_type<GLuint>
    : std::integral_constant<GLenum, GL_UNSIGNED_INT> {};

} // namespace hera::gl::detail

#endif
