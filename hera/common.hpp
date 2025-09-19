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
#include <thread>
#include <tuple>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ranges>
#include <optional>
#include <concepts>
#include <type_traits>
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
#include <shared_mutex>
#include <mutex>

#include <cstddef>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define QUILL_DISABLE_NON_PREFIXED_MACROS
#include <quill/Logger.h>
#include <quill/LogMacros.h>
#include <quill/std/FilesystemPath.h>
#include <quill/std/Vector.h>
#include <quill/std/UnorderedMap.h>
#include <quill/bundled/fmt/format.h>
#include <quill/bundled/fmt/ostream.h>
#include <quill/DirectFormatCodec.h>
#include <quill/DeferredFormatCodec.h>

#include <oneapi/tbb/concurrent_queue.h>

#include <boost/container_hash/hash.hpp>

// needs to be in global for specializations
namespace fmt = fmtquill;

namespace hera {

extern quill::Logger* global_log;
using quill::LogLevel;

/*
 * ==[[namespace imports]]==
 */
using namespace std::ranges;
namespace ranges = std::ranges;
namespace views = std::views;
namespace fs = std::filesystem;
namespace chrono = std::chrono;
namespace numbers = std::numbers;

/*
 * ==[[stdlib]]==
 */
using fs::path;
using std::array;
using std::back_insert_iterator;
using std::back_inserter;
using std::bitset;
using std::fstream;
using std::function;
using std::ifstream;
using std::initializer_list;
using std::ios_base;
using std::iostream;
using std::istream;
using std::istringstream;
using std::map;
using std::multimap;
using std::mutex;
using std::nullopt;
using std::nullptr_t;
using std::ofstream;
using std::optional;
using std::ostream;
using std::ostream_iterator;
using std::ostreambuf_iterator;
using std::ostringstream;
using std::pair;
using std::ptrdiff_t;
using std::ratio;
using std::scoped_lock;
using std::set;
using std::shared_lock;
using std::shared_mutex;
using std::shared_ptr;
using std::size_t;
using std::span;
using std::stack;
using std::string;
using std::string_view;
using std::stringstream;
using std::swap;
using std::tuple;
using std::u32string;
using std::u32string_view;
using std::unique_lock;
using std::unique_ptr;
using std::unordered_map;
using std::unordered_multimap;
using std::unordered_multiset;
using std::unordered_set;
using std::variant;
using std::vector;
using std::weak_ptr;

using std::string_literals::operator""s;
using std::string_view_literals::operator""sv;

// highest resolution steady clock type.
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

using uint128_t = unsigned __int128;
using int128_t = __int128;

using std::bit_cast;
using std::declval;

/*
 * ==[[GLM]]==
 */
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

/*
 * ==[[oneTBB]]==
 */
using oneapi::tbb::concurrent_queue;

/*
 * ==[[concepts and traits]]==
 */
using std::convertible_to;
using std::decay_t;
using std::derived_from;
using std::invocable;
using std::remove_const_t;
using std::remove_cv_t;
using std::remove_cvref_t;
using std::remove_pointer_t;
using std::remove_reference_t;
using std::same_as;

template<typename T>
concept byte_like =
    same_as<remove_cv_t<T>, char> || same_as<remove_cv_t<T>, signed char> ||
    same_as<remove_cv_t<T>, unsigned char> ||
    same_as<remove_cv_t<T>, std::byte>;

template<size_t I>
using size_constant = std::integral_constant<size_t, I>;

template<typename R, typename Fn, typename... Args>
concept invocable_r = requires(Fn&& fn, Args&&... args) {
    std::invoke_r<R>(std::forward<Fn>(fn), std::forward<Args>(args)...);
};

// provides the type U with the qualifiers of T
template<typename T, typename U>
using like_t = decltype(std::forward_like<T>(declval<U>()));

template<typename T, typename U>
using copy_cv = remove_reference_t<like_t<T, U>>;

/*
 * ==[tuple concepts and traits]==
 */
using std::tuple_element;
using std::tuple_element_t;
using std::tuple_size;
using std::tuple_size_v;

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

/*
 * ==[[range concepts and type traits]]==
 */
using ranges::contiguous_range;
using ranges::range;
using ranges::range_value_t;
using ranges::sized_range;
using ranges::view;

template<typename R>
using range_v = ranges::range_value_t<R>;

template<typename R, typename T>
concept span_of =
    contiguous_range<R> && sized_range<R> && same_as<range_value_t<R>, T>;

template<typename R>
concept spanner = contiguous_range<R> && sized_range<R>;

template<std::indirectly_readable T>
using iter_const_reference_t =
    std::common_reference_t<const std::iter_value_t<T>&&,
                            std::iter_reference_t<T>>;

template<ranges::range R>
using range_const_reference_t = iter_const_reference_t<ranges::iterator_t<R>>;

template<typename T>
concept constant_iterator =
    std::input_iterator<T> &&
    same_as<std::iter_reference_t<T>, iter_const_reference_t<T>>;

template<typename R>
concept constant_range =
    ranges::input_range<R> && constant_iterator<ranges::iterator_t<R>>;

template<typename R>
concept resizable_range =
    ranges::range<R> && requires(R r, size_t n) { r.resize(n); };

template<typename R>
concept byte_buffer = contiguous_range<R> && (!constant_range<R>) &&
                      resizable_range<R> && byte_like<range_v<R>>;

template<typename T>
struct element_type {
    using type = T;
};

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

// size in bytes of any range that models `span_of`
template<typename R, typename T = range_value_t<R>>
    requires span_of<R, T>
constexpr size_t size_bytes(const R& r)
{
    return ranges::size(r) * sizeof(T);
}

/*
 * ==[[hash support]]==
 */

// transparent hasher
template<typename T>
struct trans_hash : boost::hash<T> {
    using is_transparent = void;
};

template<typename T>
using borrowed_type = std::conditional_t<same_as<T, string>, string_view, T>;

template<typename K, typename T, typename H = borrowed_type<K>,
         typename C = std::equal_to<>>
using hash_map = unordered_map<K, T, trans_hash<H>, C>;

template<typename K, typename T, typename H = borrowed_type<K>,
         typename C = std::equal_to<>>
using hash_multimap = unordered_multimap<K, T, trans_hash<H>, C>;

template<typename T, typename H = borrowed_type<T>,
         typename C = std::equal_to<>>
using hash_set = unordered_set<T, trans_hash<H>, C>;

template<typename T, typename H = borrowed_type<T>,
         typename C = std::equal_to<>>
using hash_multiset = unordered_multiset<T, trans_hash<H>, C>;

/*
 * ==[[bitfield support]]==
 */

// specialize to enable bitfield operations
template<typename T>
struct enable_bitfield;

template<typename T>
concept bitfield = requires {
    typename enable_bitfield<T>::value;
    typename std::underlying_type_t<T>;
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

struct format_parser {
    bool alternate = false;

    template<typename ParseCtx>
    constexpr ParseCtx::iterator parse(ParseCtx& ctx)
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it == end) {
            return it;
        }
        for (; it != end && *it != ':' && *it != '}'; ++it) {
            switch (*it) {
            case '#':
                alternate = true;
                break;
            default:
                throw fmt::format_error("bad parse args");
            }
        }
        return it;
    }
};
} // namespace hera

// logging macros

#define LOG_TRACE_L3(fmt, ...)                                                 \
    QUILL_LOG_TRACE_L3(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_TRACE_L2(fmt, ...)                                                 \
    QUILL_LOG_TRACE_L2(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_TRACE_L1(fmt, ...)                                                 \
    QUILL_LOG_TRACE_L1(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)                                                    \
    QUILL_LOG_DEBUG(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) QUILL_LOG_INFO(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...)                                                  \
    QUILL_LOG_WARNING(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)                                                    \
    QUILL_LOG_ERROR(hera::global_log, fmt, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...)                                                 \
    QUILL_LOG_CRITICAL(hera::global_log, fmt, ##__VA_ARGS__)

// value-based macros

#define LOGV_TRACE_L3(fmt, message, ...)                                       \
    QUILL_LOGV_TRACE_L3(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_TRACE_L2(fmt, message, ...)                                       \
    QUILL_LOGV_TRACE_L2(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_TRACE_L1(fmt, message, ...)                                       \
    QUILL_LOGV_TRACE_L1(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_DEBUG(fmt, message, ...)                                          \
    QUILL_LOGV_DEBUG(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_INFO(fmt, message, ...)                                           \
    QUILL_LOGV_INFO(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_WARNING(fmt, message, ...)                                        \
    QUILL_LOGV_WARNING(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_ERROR(fmt, message, ...)                                          \
    QUILL_LOGV_ERROR(hera::global_log, fmt, message, ##__VA_ARGS__)
#define LOGV_CRITICAL(fmt, message, ...)                                       \
    QUILL_LOGV_CRITICAL(hera::global_log, fmt, message, ##__VA_ARGS__)

#endif
