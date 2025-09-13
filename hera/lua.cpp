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

#include <cpptrace/cpptrace.hpp>
#include <sol/error.hpp>
#include <sol/forward.hpp>
#include <sol/optional_implementation.hpp>
#include <sol/protected_function_result.hpp>

#include <hera/common.hpp>
#include <hera/log.hpp>
#include <hera/lua.hpp>
#include <hera/error.hpp>

namespace hera {

namespace {
extern "C" {

static int atpanic(lua_State* L)
{
    size_t msgsize;
    const char* msg = lua_tolstring(L, -1, &msgsize);
    if (msg) {
        std::string emsg(msg, msgsize);
        lua_settop(L, 0);
        LOG_CRITICAL("lua panic: {}", emsg);
        throw hera::runtime_error(std::move(emsg));
    }
    lua_settop(L, 0);

    LOG_CRITICAL("unexpected lua panic");
    throw hera::runtime_error("unexpected lua panic");
}

static int atexception(lua_State* L, sol::optional<const std::exception&>,
                       string_view what)
{
    LOG_ERROR("lua exception: {}", what);
    lua_pushlstring(L, what.data(), what.size());
    return 1;
}

static int traceback_error_handler(lua_State* L)
{
    std::string msg = "unknown lua error";
    sol::optional<string_view> maybetopmsg =
        sol::stack::unqualified_check_get<string_view>(L, 1, &sol::no_panic);
    if (maybetopmsg) {
        const string_view& topmsg = maybetopmsg.value();
        msg.assign(topmsg.data(), topmsg.size());
    }
    luaL_traceback(L, L, msg.c_str(), 1);
    sol::optional<string_view> maybetraceback =
        sol::stack::unqualified_check_get<string_view>(L, -1, &sol::no_panic);
    if (maybetraceback) {
        const string_view& traceback = maybetraceback.value();
        msg.assign(traceback.data(), traceback.size());
    }
    LOG_ERROR("lua traceback: {}", msg);
    return sol::stack::push(L, msg);
}
} // extern "C"
} // namespace

sol::protected_function_result script_eh(lua_State* L,
                                         sol::protected_function_result result)
{
    sol::type t = sol::type_of(L, result.stack_index());
    std::string err;
    err += sol::to_string(result.status());
    err += " error:";

    std::exception_ptr eptr = std::current_exception();
    if (eptr) {
        err += " (exception: ";
        try {
            std::rethrow_exception(eptr);
        }
        catch (const cpptrace::exception& ex) {
            err += "cpptrace::exception)[";
            err.append(ex.what());
        }
        catch (const std::exception& ex) {
            err += "std::exception)[";
            err.append(ex.what());
        }
        catch (const std::string& message) {
            err += "string)[";
            err.append(message);
        }
        catch (const char* message) {
            err += "char*)[";
            err.append(message);
        }
        catch (...) {
            err += "unknown)[";
        }
        err += "]";
    }

    if (t == sol::type::string) {
        err += ": ";
        string_view serr =
            sol::stack::unqualified_get<string_view>(L, result.stack_index());
        err.append(serr.data(), serr.size());
    }
    LOG_CRITICAL("lua error: {}", err);
    // replacing information of stack error into pfr
    int target = result.stack_index();
    if (result.pop_count() > 0) {
        sol::stack::remove(L, target, result.pop_count());
    }
    sol::stack::push(L, err);
    int top = lua_gettop(L);
    int towards = top - target;
    if (towards != 0) {
        lua_rotate(L, top, towards);
    }
    throw hera::runtime_error(std::move(err));
}

sol::state newlua()
{
    sol::state state(&atpanic);
    state.set_exception_handler(&atexception);
    lua_CFunction default_eh = sol::c_call<decltype(&traceback_error_handler),
                                           &traceback_error_handler>;
    sol::protected_function::set_default_handler(
        sol::object(state, sol::in_place, default_eh));

    return state;
}

} // namespace hera
