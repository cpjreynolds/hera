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

#ifndef HERA_IO_ROUTER_HPP
#define HERA_IO_ROUTER_HPP

#include <hera/common.hpp>
#include <hera/io/link.hpp>

namespace hera {

template<byte_buffer Container>
void slurp(istream& input, Container& buf)
{
    input.seekg(0, istream::end);
    buf.resize(input.tellg());
    input.seekg(0, istream::beg);
    auto* dst = reinterpret_cast<char*>(ranges::data(buf));
    input.read(dst, ranges::size(buf));
    buf.resize(input.gcount());
}

// a router can resolve the urls it accepts
struct router {
    virtual ~router() = default;

    virtual path resolve(const link&) const = 0;
};

struct fs_router : public router {
    // domain -> path
    hash_map<string, string> domains;

    path resolve(const link&) const override;

    // create a new `fs_router` from toml config
    fs_router(const toml::table& tbl)
    {
        auto doms = tbl.at("domain").as_array();
        if (!doms) {
            throw runtime_error("bad provider");
        }
        doms->for_each(
            [&](const toml::table& dom) { domains.emplace(make_domain(dom)); });
        LOG_DEBUG("fs_router: {}", fmt::format("{}", domains));
    }

    static pair<string, string> make_domain(const toml::table& dom)
    {
        return {dom["id"].value_or("null"), dom["path"].value_or("null")};
    }
};

class route_table {
    struct node {
        hash_map<string, node> children;
        unique_ptr<router> rtr;

        size_t size() const;
    };
    node root;
    using map_type = decltype(node::children);

    friend struct fmt::formatter<route_table>;

    pair<map_type*, map_type::iterator> maybe_at(const link& key);

    auto* node_at(this auto& self, const link& key)
    {
        auto* n = &self.root;
        for (auto part : key.parts()) {
            n = &n->children[part]; // deliberately create if missing
        }
        return n;
    }

    route_table() = default;

public:
    template<typename T, typename... Args>
        requires derived_from<T, router> && constructible_from<T, Args...>
    void emplace(const link& key, Args&&... args)
    {
        LOG_DEBUG("{}", fmt::format("{}: {}", key, key.parts()));
        node_at(key)->rtr = std::make_unique<T>(args...);
    }
    void insert(const link& key, unique_ptr<router> val);
    void insert(unique_ptr<router> val);

    const router* find(const link& key) const;
    const router& at(const link& key) const;

    // delete the router at this key
    size_t erase(const link& key);
    // delete all routers at or below this key
    size_t prune(const link& key);

    bool empty() const { return !root.rtr || root.children.empty(); }
    void clear()
    {
        root.rtr.reset();
        root.children.clear();
    }
    size_t size() const { return root.size(); }

    static route_table& get();

    static path resolve(const link& key);
};

} // namespace hera

template<>
struct fmt::formatter<hera::route_table> : hera::format_parser {
    auto format(const hera::route_table& val, auto& ctx) const
    {
        using node = hera::route_table::node;

        std::vector<std::string> prefixes;

        std::function<void(std::string, const node&)> traverse;
        traverse = [&traverse, &prefixes](std::string prefix, const node& n) {
            if (n.rtr) {
                prefixes.emplace_back(prefix);
            }
            for (auto& child : n.children) {
                LOG_DEBUG("child: {}", child.first);

                traverse(std::format("{}/{}", prefix, child.first),
                         child.second);
            }
        };
        for (auto& ch : val.root.children) {
            traverse(std::format("{}:/", ch.first), ch.second);
        }
        return std::format_to(ctx.out(), "{}", prefixes);
    }
};

template<>
struct quill::Codec<hera::route_table>
    : quill::DirectFormatCodec<hera::route_table> {};

#endif
