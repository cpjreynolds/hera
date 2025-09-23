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
using urls::authority_view;
using urls::decode_view;
using urls::url;
using urls::url_base;
using urls::url_view;
using urls::url_view_base;

/*
 * Types of URLs:
 *
 *  * relative (uses context stack)
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
    link(url&& u) : loc{std::move(u)} { validate(); };
    link(const url_view_base& u) : loc{u} { validate(); };

    template<typename S>
        requires convertible_to<const S&, string_view>
    link(const S& s);

    // resolve the link to a filepath. optionally reloading the cached value
    const path& resolve(bool reload = false) const;

    /*
     * Context
     */

    // convenience function for creating a `link` and resolving it in one call
    static path apply(string_view);
    // push a url to the context stack
    static void push(const url_view_base&);
    // push a url to the context stack
    static void push(url&&);
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

    operator const path&() const { return resolve(); }
    operator string_view() const { return resolve().native(); }

    /*
     * Queries
     */

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

    /*
     * Modifiers
     */

    // append elements to the path
    template<typename S>
        requires convertible_to<const S&, string_view>
    link& append(const S& s);
    link& append(const link&);
    link& append(const url_view_base&);

private:
    static thread_local vector<url> ctx;
    url loc;
    mutable path cached;

    // clear the cached path
    void uncache() const { cached.clear(); }
    // insert missing scheme/source
    void validate();
    static bool is_complete(const url&);
    static bool is_relative(const url&);

    url prefix() const;
};

template<typename S>
    requires convertible_to<const S&, string_view>
link::link(const S& s) : loc{s}
{
    validate();
};

template<typename S>
    requires convertible_to<const S&, string_view>
link& link::append(const S& s)
{
    return append(url_view{s});
}

// a router can resolve the urls it accepts
struct router {
    virtual ~router() = default;

    virtual string_view key() const = 0;
    virtual path resolve(const url_view_base&) const = 0;

    virtual unique_ptr<iostream> get(const url_view_base&) const = 0;
};

struct filesystem_router : public router {
    string source_id;
    // domain -> path
    hash_map<string, string> domains;

    path resolve(const url_view_base&) const override;

    // create a new `directory_router` from toml config
    filesystem_router(const toml::table& tbl)
    {
        source_id = tbl["id"].value_or("null");
        auto doms = tbl.at("domain").as_array();
        if (!doms) {
            throw runtime_error("bad provider");
        }
        doms->for_each(
            [&](const toml::table& dom) { domains.emplace(make_domain(dom)); });
    }

    static pair<string, string> make_domain(const toml::table& dom)
    {
        return {dom["id"].value_or("null"), dom["path"].value_or("null")};
    }
};

struct mount_table {
    inline static hash_map<string, unique_ptr<router>> routers;

    static path resolve(const url_view_base&);

    static void init();
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
