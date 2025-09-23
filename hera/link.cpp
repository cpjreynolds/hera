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

#include <hera/link.hpp>
#include <hera/error.hpp>
#include <hera/init.hpp>
#include <hera/config.hpp>

namespace stok = boost::urls::string_token;

namespace hera {

namespace {
void append_path(url& dst, const url_view_base& src)
{
    auto dst_path = dst.encoded_segments();
    auto src_path = src.encoded_segments();
    dst_path.insert(dst_path.end(), src_path.begin(), src_path.end());
}

} // namespace

thread_local vector<url> link::ctx{url{"hera://core"}};

url link::prefix() const
{
    auto find_complete = [](const url& l) { return is_complete(l); };
    // always at least length=1
    auto pseq = ranges::find_last_if(ctx, find_complete);
    url result = pseq.front();
    pseq.advance(1);
    for (auto& pat : pseq) {
        append_path(result, pat);
    }
    return result;
}

bool link::is_complete(const url& u)
{
    return u.has_scheme() && !u.encoded_authority().empty();
}

bool link::is_relative(const url& u)
{
    return !u.has_scheme() && u.encoded_authority().empty();
}

void link::validate()
{
    if (!loc.has_scheme() && !loc.encoded_authority().empty()) {
        // network-relative url
        loc.set_scheme("hera");
    }
    if (loc.has_scheme() && loc.encoded_authority().empty()) {
        loc.set_host("core");
    }
}

link& link::append(const link& l)
{
    append_path(loc, l.loc);
    return *this;
}

const path& link::resolve(bool reload) const
{
    if (!cached.empty() && !reload) {
        return cached;
    }
    if (is_complete()) {
        cached = mount_table::resolve(loc);
    }
    else if (is_relative()) {
        url pre = prefix();
        append_path(pre, loc);
        cached = mount_table::resolve(pre);
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

void link::push(const url_view_base& u)
{
    auto& loc = ctx.emplace_back(u);
    loc.remove_query().remove_fragment();
}

void link::push(url&& u)
{
    auto& loc = ctx.emplace_back(std::move(u));
    loc.remove_query().remove_fragment();
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

bool link::is_complete() const
{
    return is_complete(loc);
}

bool link::is_relative() const
{
    return is_relative(loc);
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

path filesystem_router::resolve(const url_view_base& u) const
{
    auto segs = u.encoded_segments();
    auto sview = ranges::subrange(segs, segs.size());
    string domain = sview.front().decode();
    sview.advance(1);
    path rv;
    if (auto loc = domains.find(domain); loc != domains.end()) {
        rv = loc->second;
    }
    else {
        LOG_ERROR("missing domain: {}", u.data());
        throw runtime_error{"missing domain"};
    }
    for (auto seg : sview) {
        decode_view dv(seg);
        rv.append(dv.begin(), dv.end());
    }
    return rv;
}

void mount_table::init()
{
    Config cfg;
    const toml::array& cfg_provs = cfg.at("provider");

    cfg_provs.for_each([](const toml::table& elt) {
        routers.emplace(elt["id"].value_or("null"),
                        std::make_unique<filesystem_router>(elt));
    });

    for (const auto& elt : routers) {
        LOG_DEBUG("{}", elt.first);
    }
}

path mount_table::resolve(const url_view_base& u)
{
    string src = u.host_name();
    if (auto rt = routers.find(src); rt != routers.end()) {
        return rt->second->resolve(u);
    }
    else {
        LOG_ERROR("unresolvable: {}", u.data());
        throw runtime_error{"unresolvable url"};
    }
}

void init::link()
{
    mount_table::init();
}

struct route_key {
    string key;
};

struct routing_table {
    hash_map<string, unique_ptr<router>> routers;

    router* find() const;
};

} // namespace hera
