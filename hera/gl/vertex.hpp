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

#ifndef HERA_VERTEX_HPP
#define HERA_VERTEX_HPP

#include <hera/common.hpp>
#include <hera/error.hpp>
#include <hera/format.hpp>
#include <hera/gl/common.hpp>
#include <hera/gl/detail.hpp>

namespace hera::gl {

// layout of an attribute within a vertex
struct AttributeFormat {
    GLuint index;
    GLint size;
    GLenum type;
    GLsizei stride;
    size_t offset;
};

// specifies that the object layout of Ts... would not require padding bytes.
template<typename... Ts>
inline constexpr bool is_packed = (sizeof(Ts) + ...) == sizeof(tuple<Ts...>);

template<typename... Ts>
concept packed = is_packed<Ts...>;

// specifies a type may be interpreted as an OpenGL vertex attribute.
template<typename T>
concept attribute = std::is_trivial_v<T> && std::is_standard_layout_v<T> &&
                    (gl_type<T> || (gl_type<element_t<T>> && array_like<T> &&
                                    lengthof(T{}) > 1 && lengthof(T{}) <= 4));

namespace detail {
// returns a compile-time constant array of attribute format descriptors for
// the given types in the given order.
template<typename... Ts>
    requires packed<Ts...> && (attribute<Ts> && ...)
consteval array<AttributeFormat, sizeof...(Ts)> make_attributes()
{
    GLuint index = 0;
    size_t offset = 0;
    constexpr GLsizei stride = (sizeof(Ts) + ... + 0);
    auto attrfn = [&]<typename TT> consteval -> AttributeFormat {
        AttributeFormat attr{
            .index = index,
            .size = lengthof(TT{}),
            .type = gl_typeof<element_t<TT>>(),
            .stride = stride,
            .offset = offset,
        };
        ++index;
        offset += sizeof(TT);
        return attr;
    };
    // this syntax is absolutely cursed.
    return { attrfn.template operator()<Ts>()... };
}

} // namespace detail

// base type for vertex<T> specializations to inherit from.
template<typename... Ts>
    requires packed<Ts...> && (attribute<Ts> && ...)
struct attributes {
    static constexpr size_t size = sizeof...(Ts);
    static constexpr std::array<AttributeFormat, sizeof...(Ts)> format =
        detail::make_attributes<Ts...>();

    using type = attributes<Ts...>;
    template<size_t I>
    using element_type = std::tuple_element_t<I, type>;
};

namespace detail {
template<typename T>
struct formattable_attributes;

template<typename... Ts>
struct formattable_attributes<attributes<Ts...>>
    : std::bool_constant<(std::formattable<Ts, char> && ...)> {};

template<typename T>
static constexpr bool formattable_attributes_v =
    formattable_attributes<T>::value;

// gets the offset in bytes for the attribute type at index I
template<size_t I, typename T>
struct attr_offset;

template<size_t I, typename T>
inline constexpr size_t attr_offset_v = attr_offset<I, T>::value;

// recursive case
template<size_t I, typename T, typename... Ts>
struct attr_offset<I, attributes<T, Ts...>>
    : std::integral_constant<
          size_t, sizeof(T) + attr_offset<I - 1, attributes<Ts...>>::value> {
    static_assert(I <= sizeof...(Ts), "attribute offset out of bounds");
};

// base case
template<typename T, typename... Ts>
struct attr_offset<0, attributes<T, Ts...>>
    : std::integral_constant<size_t, 0> {};

} // namespace detail

// specialize to designate a type as a vertex.
template<typename T>
struct vertex;

template<typename T>
struct vertex<const T> : vertex<T> {};

// specifies that a type can be used as a vertex
template<typename T>
concept is_vertex =
    std::is_standard_layout_v<T> && std::is_trivial_v<T> && requires {
        vertex<T>::size;
        vertex<T>::format;
        typename vertex<T>::type;
    };

// tuple-like get<I>(T) for vertex T
template<size_t I, typename T>
    requires is_vertex<T>
constexpr std::tuple_element_t<I, T>& get(T& v)
{
    using type = std::tuple_element_t<I, T>;
    auto offbytes = detail::attr_offset_v<I, typename vertex<T>::type>;
    auto base = reinterpret_cast<std::byte*>(&v);
    return reinterpret_cast<type&>(*(base + offbytes));
}

template<size_t I, typename T>
    requires is_vertex<T>
constexpr std::tuple_element_t<I, T> const& get(const T& v)
{
    using type = std::tuple_element_t<I, T>;
    auto offbytes = detail::attr_offset_v<I, typename vertex<T>::type>;
    auto base = reinterpret_cast<const std::byte*>(&v);
    return reinterpret_cast<const type&>(*(base + offbytes));
}

} // namespace hera::gl

// ADL doesn't work so inject into the global namespace.
using hera::gl::get;

// recurse
template<size_t I, typename T, typename... Ts>
struct std::tuple_element<I, hera::gl::attributes<T, Ts...>>
    : std::tuple_element<I - 1, hera::gl::attributes<Ts...>> {};

// base case
template<typename T, typename... Ts>
struct std::tuple_element<0, hera::gl::attributes<T, Ts...>> {
    using type = T;
};

// tuple protocol for any type that specializes vertex
template<typename T>
    requires hera::gl::is_vertex<T>
struct std::tuple_size<T>
    : std::integral_constant<size_t, hera::gl::vertex<T>::size> {};

template<size_t I, typename T>
    requires hera::gl::is_vertex<T>
struct std::tuple_element<I, T>
    : std::tuple_element<I, typename hera::gl::vertex<T>::type> {};

// std::formatter for any vertex specialization with formattable attributes.
template<typename T>
    requires hera::gl::is_vertex<T> &&
             hera::gl::detail::formattable_attributes_v<
                 typename hera::gl::vertex<T>::type>
struct fmt::formatter<T> : hera::format_parser<> {
    template<size_t... I>
    auto format_impl(const T& v, auto& ctx, std::index_sequence<I...>) const
    {
        auto strings = {fmt::format("{}", get<I>(v))...};

        auto output = ctx.out();

        auto it = strings.begin();
        for (; it != std::prev(strings.end()); ++it) {
            output = std::copy(it->begin(), it->end(), output);
        }
        return std::copy(it->begin(), it->end(), output);
    }

    auto format(const T& v, auto& ctx) const
    {
        return format_impl(
            v, ctx, std::make_index_sequence<hera::gl::vertex<T>::size>{});
    }
};

template<>
struct fmt::formatter<hera::gl::AttributeFormat> : hera::format_parser<> {
    auto format(const hera::gl::AttributeFormat& val, auto& ctx) const
    {
        auto tystr = hera::gl::gl_str(val.type);
        return std::format_to(
            ctx.out(),
            "[index: {}, size: {}, type: {}, stride: {}, offset: {}]",
            val.index, val.size, tystr, val.stride, val.offset);
    }
};

template<typename T>
    requires hera::gl::is_vertex<T> && std::formattable<T, char>
struct fmt::formatter<T> : hera::format_parser<> {
    auto format(const T& v, auto& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", v);
    }
};

#endif
