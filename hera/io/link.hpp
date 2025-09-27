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

#ifndef HERA_LINK_HPP
#define HERA_LINK_HPP

#include <hera/common.hpp>
#include <hera/config.hpp>

#include <boost/url.hpp>

namespace hera {

namespace urls = boost::urls;
using urls::decode_view;
using urls::pct_string_view;
using urls::url;
using urls::url_base;
using urls::url_view;
using urls::url_view_base;

class parts_view;

/*
 * Types of URLs:
 *
 *  * relative (uses context stack) | no scheme && no authority
 *      - path/to/file
 *      - /path/to/file
 *
 *  * source-relative: (assumes source=core)
 *      - hera:///domain/file
 *      - hera:/domain/file
 *      - hera:domain/file
 *
 *  * Complete:
 *
 *      - hera://source/domain/file
 *      - hera://source
 *      - //source/domain/file (assumes scheme=hera)
 */
class link {
public:
    link() noexcept = default;
    link(url&& u) : loc{std::move(u)} { validate(); }
    link(const url_view_base& u) : loc{u} { validate(); };

    template<convertible_to<string_view> S>
    link(const S& s) : loc{s}
    {
        validate();
    };

    link(const link&) = default;
    link(link&&) = default;

    link& operator=(const link&) = default;
    link& operator=(link&&) = default;

    // resolve the link to a concrete filesystem path.
    const path& resolve(bool reload = false) const;

    /*
     * Context
     */

    // convenience function for creating a `link` and resolving it in one call
    static path apply(string_view);
    // push a link to the context stack
    // static void push(link&&);
    template<typename T>
        requires constructible_from<link, T>
    static void push(T&& val);
    // pop the topmost element from context.
    static void pop();
    // push `this` to the context stack.
    void push() const;
    // move `this` onto the context stack
    void push() &&;

    /*
     * Conversions
     */
    const url_view_base* operator->() const { return &loc; }
    url_base* operator->()
    {
        uncache();
        return &loc;
    }
    const path& operator*() const { return resolve(); }

    /*
     * Queries
     */

    // true if link has implicit or explicit package
    bool has_package() const;
    // true if link uses the context-implicit package
    bool has_implicit_package() const;
    // true if link uses the context-implicit package
    bool has_explicit_package() const;
    // return true if link has scheme/source/domain
    bool is_complete() const;
    // return true if link is just a path
    bool is_relative() const;
    // return true if empty
    bool empty() const { return loc.empty(); }

    /*
     * Decomposition
     */

    // returns a link to the parent directory
    link parent_path() const;
    string filename() const;
    string extension() const;
    string stem() const;

    pct_string_view package() const;
    pct_string_view scheme() const;

    /*
     * Modifiers
     */

    // append elements to the path
    template<typename S>
        requires constructible_from<urls::segments_encoded_view, S>
    link& append(const S& src);
    // append elements to the path
    link& append(const link& s) { return append(s->encoded_segments()); }

    // append elements to the path
    template<typename S>
        requires constructible_from<urls::segments_encoded_view, S>
    link& operator/=(const S& src);
    // append elements to the path
    link& operator/=(const link& s) { return append(s->encoded_segments()); }

    /*
     * Iterators
     */

    friend class parts_view;
    parts_view parts() const;

private:
    static thread_local vector<link> ctx;

    url loc;
    mutable path cached;

    // clear the cached path
    void uncache() const { cached.clear(); }
    static pct_string_view implicit_package();

    link prefix() const;

    // ensure all links have schemes
    void validate()
    {
        if (!loc.has_scheme() && has_package()) {
            loc.set_scheme("hera");
        }
    }
};

// append elements to the path
template<typename S>
    requires constructible_from<urls::segments_encoded_view, S>
link& link::append(const S& src)
{
    auto srcp = urls::segments_encoded_view{src};
    auto dst = loc.encoded_segments();
    dst.insert(dst.end(), srcp.begin(), srcp.end());
    return *this;
}

template<typename T>
    requires constructible_from<link, T>
void link::push(T&& val)
{
    ctx.emplace_back(std::forward<T>(val))->remove_query().remove_fragment();
}
// append elements to the path
template<typename S>
    requires constructible_from<urls::segments_encoded_view, S>
link& link::operator/=(const S& src)
{
    auto srcp = urls::segments_encoded_view{src};
    auto dst = loc.encoded_segments();
    dst.insert(dst.end(), srcp.begin(), srcp.end());
    return *this;
}

class parts_view : public ranges::view_interface<parts_view> {
    const link* l;

public:
    parts_view(const link& l) noexcept : l{&l} {}

    class iterator;

    iterator begin() const;
    iterator end() const;

    size_t size() const { return 2 + l->loc.encoded_segments().size(); }
};

class parts_view::iterator
    : public proxy_iterator_interface<std::bidirectional_iterator_tag, string,
                                      string_view> {
    using path_iter_t = urls::segments_encoded_view::iterator;
    const link* base;
    path_iter_t path_iter;
    size_t state = 0;

    struct end_t {
        explicit end_t() = default;
    };
    struct begin_t {
        explicit begin_t() = default;
    };

    iterator(const link* l, begin_t)
        : base{l},
          path_iter{l->loc.encoded_segments().begin()} {};

    iterator(const link* l, end_t)
        : base{l},
          path_iter{l->loc.encoded_segments().end()},
          state{l->loc.encoded_segments().size() + 2} {};

    friend class parts_view;

public:
    iterator() = default;

    iterator& operator++();
    iterator& operator--();
    using iterator_interface::operator++;
    using iterator_interface::operator--;

    string_view operator*() const;

    bool operator==(const iterator& rhs) const;
};
} // namespace hera

template<>
struct fmt::formatter<hera::link> : hera::format_parser {
    auto format(const hera::link& val, auto& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", val->buffer());
    }
};

template<>
struct quill::Codec<hera::link> : quill::DirectFormatCodec<hera::link> {};

#endif
