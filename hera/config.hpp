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
#ifndef HERA_CONFIG_HPP
#define HERA_CONFIG_HPP

#include <hera/common.hpp>
#include <hera/toml.hpp>
#include <hera/error.hpp>
#include <hera/utility.hpp>
#include <hera/init.hpp>

namespace hera {

class Config {
public:
    friend struct hera::init;
    using table_t = toml::table;

private:
    static table_t* global_table;
    const table_t& _table;

    struct node_wrap {
        template<typename T>
        node_wrap(T&& data) : _data{std::forward<T>(data)} {};

        operator const toml::table&() const
        {
            if (auto tbl = _data.as_table(); tbl) {
                return *tbl;
            }
            else {
                throw hera::runtime_error("bad cast to toml::table");
            }
        }

        operator const toml::array&() const
        {
            if (auto tbl = _data.as_array(); tbl) {
                return *tbl;
            }
            else {
                throw hera::runtime_error("bad cast to toml::array");
            }
        }

        template<typename T>
        operator T() const
        {
            optional<T> maybe = nullopt;
            if constexpr (toml_native<T>) {
                maybe = _data.value_exact<T>();
            }
            else if constexpr (toml_nativeish<T>) {
                maybe = _data.value<T>();
            }
            else {
                maybe = _data.visit(
                    []<typename V>(const toml::value<V>& v) -> optional<T> {
                        if constexpr (std::convertible_to<V, T> ||
                                      std::constructible_from<T, V>) {
                            return T(v.get());
                        }
                        else {
                            return nullopt;
                        }
                    });
            }
            if (!maybe) {
                ostringstream msg;
                msg << "bad cast to " << hera::type_of<T>() << " from '"
                    << _data << "'";
                throw hera::runtime_error(std::move(msg).str());
            }
            return *maybe;
        }

    private:
        toml::node_view<const toml::node> _data;
    };

public:
    Config() : _table{*global_table} {};
    Config(const table_t& t) : _table{t} {};

    template<toml_path P>
    node_wrap at(P&& pat) const
    {
        return node_wrap(_table.at_path(pat));
    }

    template<toml_path P>
    node_wrap operator[](P&& pat) const
    {
        return at(pat);
    }

    const table_t* operator->() const { return &_table; }
    const table_t& operator*() const { return _table; }
};

} // namespace hera

#endif
