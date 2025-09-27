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

#include <hera/config.hpp>
#include <hera/error.hpp>
#include <hera/init.hpp>
#include <hera/io/link.hpp>

#include <hera/io/router.hpp>

namespace stok = boost::urls::string_token;

namespace hera {

/*
 * ==[[link]]==
 */

// the context stack used for relative links
thread_local vector<link> link::ctx{link{"hera://core"}};

// return the current package name that will be used for implicit-package links
pct_string_view link::implicit_package()
{
    auto pkg_names =
        views::transform(ctx, [](auto&& v) { return v->encoded_authority(); });
    auto pkg =
        ranges::find_last_if_not(pkg_names, &urls::pct_string_view::empty);

    return pkg.front();
}

link link::prefix() const
{
    // always at least length=1
    auto pseq = ranges::find_last_if(ctx, &link::is_complete);
    link result = pseq.front();
    pseq.advance(1);
    for (auto& pat : pseq) {
        result.append(pat);
    }
    return result;
}

const path& link::resolve(bool reload) const
{
    if (!cached.empty() && !reload) {
        return cached;
    }
    if (is_complete()) {
        LOG_DEBUG("resolving complete link: {}", loc.c_str());
        cached = route_table::resolve(*this);
    }
    else if (is_relative()) {
        LOG_DEBUG("resolving relative link: {}", *this);
        auto prelim = prefix();
        prelim.append(loc);
        cached = route_table::resolve(prelim);
    }
    else {
        throw runtime_error{"attempt to resolve invalid link"};
    }
    return cached;
}

path link::apply(string_view s)
{
    return link{s}.resolve();
}

void link::push() const
{
    push(loc);
}

void link::push() &&
{
    ctx.emplace_back(std::move(loc));
}

void link::pop()
{
    // keep default
    if (ctx.size() > 1) {
        ctx.pop_back();
    }
}

bool link::has_package() const
{
    return has_explicit_package() || has_implicit_package();
}

bool link::has_implicit_package() const
{
    return loc.has_scheme() && !has_explicit_package();
}

bool link::has_explicit_package() const
{
    return !loc.encoded_authority().empty();
}

bool link::is_complete() const
{
    return has_package();
}

bool link::is_relative() const
{
    return !loc.has_scheme() && !has_explicit_package();
}

pct_string_view link::package() const
{
    if (has_implicit_package()) {
        return implicit_package();
    }
    else {
        return loc.encoded_authority();
    }
}

link link::parent_path() const
{
    link rv = *this;
    rv->encoded_segments().pop_back();
    rv.uncache();
    return rv;
}

string link::filename() const
{
    return loc.segments().back();
}

string link::stem() const
{
    auto fname = filename();
    if (auto pos = fname.rfind('.'); pos != fname.npos) {
        fname.erase(pos);
    }
    return fname;
}

string link::extension() const
{
    auto fname = filename();
    auto pos = fname.rfind('.');
    fname.erase(0, pos);
    return fname;
}

parts_view link::parts() const
{
    return parts_view{*this};
}

/*
 * ==[[parts_view]]==
 */

parts_view::iterator parts_view::begin() const
{
    return {l, iterator::begin_t{}};
}

parts_view::iterator parts_view::end() const
{
    return {l, iterator::end_t{}};
}

parts_view::iterator& parts_view::iterator::operator++()
{
    ++state;
    if (state > 2) {
        ++path_iter;
    }
    return *this;
}

parts_view::iterator& parts_view::iterator::operator--()
{
    --state;
    if (state > 1) {
        --path_iter;
    }
    return *this;
}

string_view parts_view::iterator::operator*() const
{
    if (state == 0) {
        return base->loc.scheme();
    }
    else if (state == 1) {
        return base->package();
    }
    else {
        return *path_iter;
    }
}

bool parts_view::iterator::operator==(const iterator& rhs) const
{
    return (base == rhs.base) && (state == rhs.state) &&
           (path_iter == rhs.path_iter);
};

} // namespace hera
