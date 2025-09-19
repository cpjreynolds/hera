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

namespace hera {

/*
 * a resolvable path to an asset
 */
class link {
    // invariant: always has at least the domain separator
    // simplifies the code a LOT
    string _buf{domain_separator};
    // cached resolved path
    mutable path _pat;

    // thread local context for relative paths
    static thread_local vector<link> _pathstack;

    friend fmt::formatter<link>;

public:
    static constexpr string_view domain_separator = ":/";

    // constructs an empty link
    link() = default;
    // constructs a link from a path
    link(const path&);

    // resolves the link to a concrete path
    const path& resolve(bool reload = false) const;
    operator const path&() const;

    // push a path onto the thread local path stack
    static void push(const link&);
    // pop a path from the thread local path stack
    static void pop();

    // the current prefix that will be prepended to resolved relative links
    static path prefix();

    static path apply(const path&);

    bool has_domain() const;
    bool has_location() const;
    bool has_filename() const;

    string_view filename() const;
    string_view extension() const;
    string_view stem() const;

    // returns true if the link has no domain.
    bool is_relative() const { return !has_domain(); }
    // returns true if the link has a domain.
    bool is_absolute() const { return has_domain(); }

    string_view domain() const;
    link& set_domain(string_view);

    string_view location() const;
    link& set_location(string_view);

    bool empty() const;
    void clear();

    link& append(string_view);
    link& append(const link&);

    link& operator/=(string_view s) { return append(s); }
    link& operator/=(const link& l) { return append(l); }

    link operator/(string_view) const;
    link operator/(const link&) const;
};

} // namespace hera

template<>
struct fmt::formatter<hera::link> : hera::format_parser {
    auto format(const hera::link& val, auto& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", val._buf);
    }
};

template<>
struct quill::Codec<hera::link> : quill::DirectFormatCodec<hera::link> {};

#endif
