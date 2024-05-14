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

#ifndef HERA_UTILITY_HPP
#define HERA_UTILITY_HPP

#include <source_location>

#include <hera/common.hpp>
#include <hera/error.hpp>

namespace hera {

namespace detail {
template<typename T>
consteval auto func_name()
{
    const auto& loc = std::source_location::current();
    return loc.function_name();
}

template<typename T>
consteval string_view type_of_impl()
{
    constexpr string_view fname = func_name<T>();
    return {fname.begin() + fname.find('=') + 2, fname.end() - 1};
}
} // namespace detail

template<typename T>
consteval string_view type_of()
{
    return detail::type_of_impl<T>();
}

template<typename T, typename Pat, typename... Pats>
constexpr string type_of(Pat without, Pats... rest)
{
    string raw{type_of<T>()};
    auto filterfn = [&raw](string_view pat) {
        for (auto loc = raw.find(pat); loc != raw.npos; loc = raw.find(pat)) {
            raw.erase(loc, pat.size());
        }
    };
    filterfn(without);
    (filterfn(rest), ...);
    return raw;
}

// read entire file into a string
string slurp(const char*);
string slurp(const string&);

// convert UTF-8 string to UTF-32
// u32string u32conv(string_view);

// return the path to the executable.
path get_exe_path();
// return the path to the executable directory.
path get_exe_dir();

// return the path to the home directory
path get_home_dir();

// return the path to the "local" directory
// UNIX: $HOME/.config/hera
// Windows: $HOME/Documents/hera
path get_local_dir();

} // namespace hera

#endif
