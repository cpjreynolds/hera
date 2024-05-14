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

#ifndef HERA_TOML_HPP
#define HERA_TOML_HPP

#include <hera/common.hpp>

#include <toml++/toml.hpp>

// inject into toml++ namespace
namespace toml {

toml::table inherit(const table& base, const table& derived);

} // namespace toml

namespace hera {

template<typename T>
concept toml_any = std::derived_from<T, toml::node> || same_as<T, toml::date> ||
                   same_as<T, toml::date_time> || same_as<T, toml::time>;

template<typename T>
concept toml_native =
    toml::impl::is_native<T> || toml::impl::can_represent_native<T>;

template<typename T>
concept toml_nativeish =
    toml_native<T> || toml::impl::can_partially_represent_native<T>;

template<typename T>
concept toml_path =
    same_as<T, toml::path> || std::convertible_to<T, string_view> ||
    std::convertible_to<T, std::wstring_view>;

} // namespace hera

template<hera::toml_any T>
struct fmt::formatter<T> : fmt::ostream_formatter {};

#endif
