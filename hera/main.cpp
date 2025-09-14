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

#include <hera/error.hpp>
#include <hera/init.hpp>
#include <hera/state.hpp>
#include <hera/toml.hpp>
#include <hera/gl/common.hpp>
#include <hera/event.hpp>

#include <hera/config.hpp>

#include <locale>

using namespace hera;

int main()
#if HERA_CATCH
try
#endif
{
    // is this a terrible idea?
    std::locale::global(std::locale(""));

    init::logging();
    init::error();
    init::config();
    init::window();
    init::gl();
    init::input();
    init::ui();

    shared_ptr<State> state = State::create();
    state->run();

    deinit::ui();
    deinit::input();
    deinit::window();

    return 0;
}
#if HERA_CATCH
catch (const cpptrace::exception& e) {
    std::cerr << "Error: " << e.message() << '\n';
    e.trace().print(std::cerr, cpptrace::isatty(cpptrace::stderr_fileno));
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
#endif
