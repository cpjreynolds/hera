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

namespace hera {

namespace {
// {domain -> canonical path}
//  e.g.:
//      "assets:/" -> "../assets"
//      "data:/" -> "assets:/data" -> "../assets/data"
static hash_map<string, path> domains;

// resolve one level of a domain.
optional<path> resolve_domain(string_view p)
{
    if (p.empty())
        return nullopt;

    if (auto loc = p.find(":/"); loc != p.npos) {
        auto dname = p.substr(0, loc);

        if (auto val = domains.find(dname); val != domains.cend()) {
            string result{p};
            result.replace(0, loc + 1, val->second);
            return path{std::move(result)};
        }
        else {
            LOG_ERROR("invalid domain: {}", dname);
            throw runtime_error{"invalid domain"};
        }
    }
    return nullopt;
}
} // namespace

void init::link()
{
    Config cfg;

    const toml::array& cfg_doms = cfg.at("domain");
    cfg_doms.for_each([](const toml::table& elt) {
        elt.at("id").visit([&](const toml::value<string>& dom) {
            elt.at("path").visit([&](const toml::value<string>& pat) {
                domains.insert({*dom, *pat});
                LOG_DEBUG("registered domain: {} = {}", *dom, *pat);
            });
        });
    });

    for (auto& node : domains) {
        while (auto newval = resolve_domain(node.second.native())) {
            node.second = *newval;
        }
    }
}

// thread_local path link::_root{"assets:/"};
thread_local vector<link> link::_pathstack;

link::link(const path& p)
{
    auto s = p.generic_string();
    auto pos = s.find(domain_separator);
    if (pos == s.npos) {
        // no domain
        append(s);
    }
    else {
        // yes domain
        _buf = std::move(s);
        /*
         * auto dom = s.substr(0, pos);
         * _buf.insert(0, dom);
         * auto loc = s.substr(pos + 2);
         * _buf.append(loc);
         */
    }
}

link& link::set_domain(string_view ndom)
{
    if (ndom.find_first_of(domain_separator) != ndom.npos) {
        // goof
        throw runtime_error{"bad domain"};
    }
    auto pos = _buf.find(domain_separator);
    _buf.replace(0, pos, ndom);
    _pat.clear();
    return *this;
}

link& link::set_location(string_view npat)
{
    auto pos = _buf.find(domain_separator);
    pos += 2;
    _buf.replace(pos, _buf.npos, npat);
    _pat.clear();
    return *this;
}

string_view link::domain() const
{
    auto pos = _buf.find(domain_separator);
    return string_view{_buf.data(), pos};
}

string_view link::location() const
{
    auto pos = _buf.find(domain_separator);
    pos += 2;
    return string_view{_buf.data() + pos, _buf.size() - pos};
}

string_view link::filename() const
{
    auto pos = _buf.rfind('/');
    return string_view{_buf}.substr(pos);
}

string_view link::extension() const
{
    auto fname = filename();
    auto pos = fname.rfind('.');
    if (pos == _buf.npos) {
        return {};
    }
    else {
        fname.remove_prefix(pos);
        return fname;
    }
}

string_view link::stem() const
{
    auto fname = filename();
    auto pos = fname.rfind('.');
    if (pos != fname.npos && pos != 0) {
        fname.remove_suffix(_buf.size() - pos);
    }
    return fname;
}

bool link::has_domain() const
{
    return !domain().empty();
}

bool link::has_location() const
{
    return !location().empty();
}

bool link::has_filename() const
{
    return !filename().empty();
}

bool link::empty() const
{
    return _buf == domain_separator;
}

void link::clear()
{
    _buf = domain_separator;
    _pat.clear();
}

link::operator const path&() const
{
    return resolve();
}

const path& link::resolve(bool reload) const
{
    if (empty()) {
        throw runtime_error{"empty link"};
    }
    if (!_pat.empty() && !reload) {
        return _pat;
    }

    if (!has_domain()) {
        // no domain. use pathstack.
        _pat = (prefix() / location()).lexically_normal();
    }
    else {
        _pat = resolve_domain(_buf)->lexically_normal();
    }
    return _pat;
}

link& link::append(string_view s)
{
    if (s.starts_with('/')) {
        s.remove_prefix(1);
        return set_location(s);
    }
    if (!_buf.ends_with('/')) {
        _buf.push_back('/');
    }
    _buf.append(s);
    _pat.clear();
    return *this;
}

link& link::append(const link& l)
{
    if (l.is_absolute()) {
        *this = l;
    }
    else {
        append(l.location());
    }
    return *this;
}

link link::operator/(string_view s) const
{
    link rv;
    rv._buf = _buf;
    rv.append(s);
    return rv;
}

link link::operator/(const link& l) const
{
    link rv;
    rv._buf = _buf;
    rv.append(l);
    return rv;
}

path link::prefix()
{
    link rv;
    for (const auto& p : _pathstack) {
        rv /= p;
    }
    if (!rv.has_domain()) {
        return path{rv.location()};
    }
    else {
        return *resolve_domain(rv._buf);
    }
}

void link::push(const link& l)
{
    _pathstack.push_back(l);
}

void link::pop()
{
    _pathstack.pop_back();
}

path link::apply(const path& p)
{
    link l{p.native()};
    return l.resolve();
}

} // namespace hera
