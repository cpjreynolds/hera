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

#include <hera/io/router.hpp>

namespace hera {

// tries to find a node. doesn't create new nodes
pair<route_table::map_type*, route_table::map_type::iterator>
route_table::maybe_at(const link& key)
{
    node* n = &root;
    map_type::iterator it;
    map_type* table = nullptr;
    for (auto part : key.parts()) {
        if (it = n->children.find(part); it == n->children.end()) {
            // not found.
            return {};
        }
        else {
            table = &n->children;
            n = &it->second;
        }
    }
    return pair{table, it};
}

void route_table::insert(const link& key, unique_ptr<router> r)
{
    node_at(key)->rtr = std::move(r);
}

const router* route_table::find(const link& key) const
{
    const node* n = &root;
    const router* best_match = root.rtr.get();
    for (auto part : key.parts()) {
        if (auto it = n->children.find(part); it != n->children.end()) {
            n = &it->second;
            if (n->rtr) {
                best_match = n->rtr.get();
            }
            continue;
        }
        break;
    }
    return best_match;
}

const router& route_table::at(const link& key) const
{
    if (auto r = find(key); r) {
        return *r;
    }
    else {
        LOG_ERROR("route_table::at: \"{}\" not found", key);
        throw out_of_range{"route_table::at: key not found"};
    }
}

size_t route_table::erase(const link& key)
{
    auto [m, it] = maybe_at(key);
    size_t rv = it->second.rtr ? 1 : 0;
    if (it->second.children.empty()) {
        m->erase(it);
    }
    else {
        it->second.rtr.reset();
    }
    return rv;
}

size_t route_table::node::size() const
{
    function<size_t(const node&)> traverse;
    traverse = [&traverse](const node& n) {
        size_t rv = n.rtr ? 1 : 0;
        for (auto& ch : n.children) {
            rv += traverse(ch.second);
        }
        return rv;
    };
    return traverse(*this);
}

size_t route_table::prune(const link& key)
{
    auto [m, it] = maybe_at(key);
    size_t rv = it->second.size();
    m->erase(it);
    return rv;
}

path fs_router::resolve(const link& u) const
{
    auto segs = u->encoded_segments();
    auto sview = ranges::subrange(segs, segs.size());
    string domain = sview.front().decode();
    sview.advance(1);
    path rv;
    if (auto loc = domains.find(domain); loc != domains.end()) {
        rv = loc->second;
    }
    else {
        LOG_ERROR("missing domain: {}", u->data());
        throw runtime_error{"missing domain"};
    }
    for (auto seg : sview) {
        decode_view dv(seg);
        rv.append(dv.begin(), dv.end());
    }
    return rv;
}

route_table& route_table::get()
{
    static route_table global{};
    return global;
}

path route_table::resolve(const link& key)
{
    return get().at(key).resolve(key);
}

void init::route_table()
{
    Config cfg;
    const toml::array& cfg_provs = cfg.at("provider");

    auto& routes = route_table::get();

    cfg_provs.for_each([&](const toml::table& elt) {
        link key{"hera://"};
        key->set_host_name(elt["id"].value_or("null"));
        routes.emplace<fs_router>(key, elt);
    });
}

} // namespace hera
