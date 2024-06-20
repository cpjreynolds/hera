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

#ifndef HERA_COMMON_HPP
#define HERA_COMMON_HPP

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <array>
#include <span>
#include <filesystem>
#include <utility>
#include <tuple>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ranges>
#include <optional>
#include <concepts>
#include <variant>
#include <set>
#include <unordered_set>
#include <stack>
#include <memory>
#include <functional>
#include <chrono>
#include <bitset>
#include <numbers>
#include <initializer_list>
#include <iterator>
#include <ratio>

#include <cstddef>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <quill/Fmt.h>

// needs to be in global for specializations
// namespace fmt = fmtquill;

namespace hera {

constexpr size_t binom(size_t n, size_t k) noexcept
{
    return (k > n) ? 0 : // out of range
               (k == 0 || k == n) ? 1
                                  : // edge
               (k == 1 || k == n - 1) ? n
                                      : // first
               (k + k < n) ?            // recursive:
               (binom(n - 1, k - 1) * n) / k
                           :                    //  path to k=1   is faster
               (binom(n - 1, k) * n) / (n - k); //  path to k=n-1 is faster
}

using uint128_t = unsigned __int128;
using int128_t = __int128;

using glm::ivec2;
using glm::ivec3;
using glm::ivec4;
using glm::mat;
using glm::mat2;
using glm::mat2x3;
using glm::mat2x4;
using glm::mat3;
using glm::mat3x2;
using glm::mat3x4;
using glm::mat4;
using glm::mat4x2;
using glm::mat4x3;
using glm::quat;
using glm::uvec2;
using glm::uvec3;
using glm::uvec4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using std::convertible_to;
using std::decay_t;
using std::declval;
using std::remove_const_t;
using std::remove_cv_t;
using std::remove_cvref_t;
using std::remove_reference_t;
using std::same_as;

using std::tuple_element;
using std::tuple_element_t;
using std::tuple_size;
using std::tuple_size_v;

using std::bit_cast;

template<typename T, typename U>
using like_t = decltype(std::forward_like<T>(declval<U>()));

template<typename T, typename U>
using copy_cv = remove_reference_t<like_t<T, U>>;

template<typename T, size_t N>
concept has_tuple_element = requires(T t) {
    typename tuple_element_t<N, remove_const_t<T>>;
    { get<N>(t) } -> std::convertible_to<const tuple_element_t<N, T>&>;
};

template<typename T>
concept tuple_like = !std::is_reference_v<T> && requires {
    typename tuple_size<T>::type;
    { tuple_size_v<T> } -> std::convertible_to<size_t>;
} && []<size_t... N>(std::index_sequence<N...>) {
    return (has_tuple_element<T, N> && ...);
}(std::make_index_sequence<tuple_size_v<T>>());

template<typename T>
concept pair_like = tuple_like<T> && tuple_size_v<T> == 2;

template<typename T>
struct element_type;

template<typename T>
    requires std::is_array_v<T>
struct element_type<T> {
    using type = std::remove_all_extents_t<T>;
};

template<typename T>
    requires(requires { typename T::value_type; })
struct element_type<T> {
    using type = T::value_type;
};

template<typename T>
using element_t = element_type<T>::type;

// consteval number of elements
template<typename T>
consteval size_t lengthof(const T& t)
{
    if constexpr (requires { typename T::length_type; }) {
        return sizeof(T) / sizeof(typename T::value_type);
    }
    else if constexpr (requires(T v) { std::size(t); }) {
        return std::size(t);
    }
    else {
        return 1;
    }
}

template<typename T, size_t N = lengthof(T{}), typename E = element_t<T>>
concept array_like = same_as<element_t<T>, E> && lengthof(T{}) == N &&
                     !requires(T t) { t.resize(); };

namespace ranges = std::ranges;
namespace views = std::views;

using ranges::contiguous_range;
using ranges::range;
using ranges::sized_range;
using ranges::view;

using ranges::range_value_t;

template<typename R>
using range_v = ranges::range_value_t<R>;

template<typename R, typename T>
concept span_of =
    contiguous_range<R> && sized_range<R> && same_as<range_value_t<R>, T>;

template<typename R>
concept spanner = contiguous_range<R> && sized_range<R>;

// size in bytes of any range that models `span_of`
template<typename R, typename T = range_value_t<R>>
constexpr size_t size_bytes(const R& r)
{
    return ranges::size(r) * sizeof(T);
}

template<size_t I>
using size_constant = std::integral_constant<size_t, I>;

using std::tuple_element;
using std::tuple_element_t;

using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;

using std::nullptr_t;
using std::ptrdiff_t;
using std::size_t;

using std::nullopt;
using std::optional;
using std::variant;

using std::back_insert_iterator;
using std::back_inserter;

using std::function;

using std::array;
using std::bitset;
using std::initializer_list;
using std::map;
using std::multimap;
using std::pair;
using std::set;
using std::span;
using std::stack;
using std::string;
using std::string_view;
using std::tuple;
using std::u32string;
using std::u32string_view;
using std::unordered_map;
using std::unordered_multimap;
using std::unordered_multiset;
using std::unordered_set;
using std::vector;

using std::string_literals::operator""s;
using std::string_view_literals::operator""sv;

template<typename T>
struct trans_hash : std::hash<T> {
    using is_transparent = void;
};

template<typename K, typename T, typename H = K, typename C = std::equal_to<>>
using hash_map = unordered_map<K, T, trans_hash<H>, C>;

template<typename K, typename T, typename H = K, typename C = std::equal_to<>>
using hash_multimap = unordered_multimap<K, T, trans_hash<H>, C>;

template<typename T, typename H = T, typename C = std::equal_to<>>
using hash_set = unordered_set<T, trans_hash<H>, C>;

template<typename T, typename H = T, typename C = std::equal_to<>>
using hash_multiset = unordered_multiset<T, trans_hash<H>, C>;

using std::fstream;
using std::ifstream;
using std::ios_base;
using std::iostream;
using std::istream;
using std::istringstream;
using std::ofstream;
using std::ostream;
using std::ostringstream;
using std::stringstream;

using std::ostream_iterator;
using std::ostreambuf_iterator;

using std::swap;

namespace fs = std::filesystem;
using fs::path;

using std::ratio;

namespace chrono = std::chrono;
using clock =
    std::conditional_t<chrono::high_resolution_clock::is_steady,
                       chrono::high_resolution_clock, chrono::steady_clock>;

using chrono::duration;
using chrono::duration_cast;

using chrono::microseconds;
using chrono::milliseconds;
using chrono::seconds;

using std::chrono_literals::operator""s;
using std::chrono_literals::operator""ms;
using std::chrono_literals::operator""us;
using std::chrono_literals::operator""ns;

namespace numbers = std::numbers;

template<typename T>
struct enable_bitfield;

template<typename T>
concept bitfield = requires {
    typename enable_bitfield<T>::value;
    typename std::underlying_type<T>::type;
};

template<bitfield T>
constexpr T operator|(T l, T r)
{
    return static_cast<T>(std::to_underlying(l) | std::to_underlying(r));
}

template<bitfield T>
constexpr T operator&(T l, T r)
{
    return static_cast<T>(std::to_underlying(l) & std::to_underlying(r));
}

template<bitfield T>
constexpr T operator^(T l, T r)
{
    return static_cast<T>(std::to_underlying(l) ^ std::to_underlying(r));
}

template<bitfield T>
constexpr T& operator|=(T& l, T r)
{
    return l = l | r;
}

template<bitfield T>
constexpr T& operator&=(T& l, T r)
{
    return l = l & r;
}

template<bitfield T>
constexpr T& operator^=(T& l, T r)
{
    return l = l ^ r;
}

} // namespace hera

#endif
