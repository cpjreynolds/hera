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

#ifndef HERA_LUA_HPP
#define HERA_LUA_HPP

#include <filesystem>

#include <sol/protected_function_result.hpp>
#include <sol/sol.hpp>

namespace sol {

template<typename Handler>
bool sol_lua_check(sol::types<std::filesystem::path>, lua_State* L, int index,
                   Handler&& handler, sol::stack::record& tracking)
{
    // indices can be negative to count backwards from the top of the stack,
    // rather than the bottom up
    // to deal with this, we adjust the index to
    // its absolute position using the lua_absindex function
    int absidx = lua_absindex(L, index);
    // Check index is the proper type
    bool success = sol::stack::check<std::string>(L, absidx, handler);
    tracking.use(1);
    return success;
}

inline std::filesystem::path sol_lua_get(sol::types<std::filesystem::path>,
                                         lua_State* L, int index,
                                         sol::stack::record& tracking)
{
    int absidx = lua_absindex(L, index);
    // Get the first element
    std::string p = sol::stack::get<std::string>(L, absidx);
    // we use 1 slot
    tracking.use(1);
    return std::filesystem::path{std::move(p)};
}

inline int sol_lua_push(lua_State* L, const std::filesystem::path& p)
{
    int amount = sol::stack::push(L, p.string());
    return amount;
}

} // namespace sol

namespace hera {

sol::protected_function_result script_eh(lua_State*,
                                         sol::protected_function_result);

sol::state newlua();

} // namespace hera

#endif
