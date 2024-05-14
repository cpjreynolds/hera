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

#include <hera/toml.hpp>

namespace {
using toml::table;

static void inherit_impl(table& base, const table& derived)
{
    derived.for_each([&](const toml::key& k, auto&& v) {
        if (base.contains(k)) {
            if constexpr (toml::is_table<decltype(v)>) {
                if (auto bt = base.get_as<table>(k)) {
                    return inherit_impl(*bt, v);
                }
            }
            else if constexpr (toml::is_array<decltype(v)>) {
                if (auto ba = base.get_as<toml::array>(k);
                    ba && ba->is_array_of_tables() && v.is_array_of_tables()) {
                    ba->insert(ba->end(), v.begin(), v.end());
                    return;
                }
            }
        }
        base.insert_or_assign(k, v);
    });
}
} // namespace

namespace toml {

table inherit(const table& base, const table& derived)
{
    table result(base);
    inherit_impl(result, derived);
    return result;
}

} // namespace toml
