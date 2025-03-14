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

#include <unistd.h>

#include <hera/common.hpp>
#include <hera/utility.hpp>
#include <hera/init.hpp>
#include <hera/log.hpp>

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/sinks/ConsoleSink.h>

namespace hera {

quill::Logger* global_log = nullptr;

void init::logging()
{
    using quill::Backend, quill::Frontend, quill::PatternFormatterOptions;

    Backend::start();

    const string pattern =
        "[%(time)][%(thread_name:^6)][%(log_level_short_code:^2)] - %(message)";
    const string timepat = "%H:%M:%S.%Qms";

    auto sink =
        Frontend::create_or_get_sink<quill::ConsoleSink>("console_sink_1");

    global_log = Frontend::create_or_get_logger(
        "root", std::move(sink), PatternFormatterOptions{pattern, timepat});

    quill::detail::set_thread_name("main");
    global_log->set_log_level(LogLevel::Debug);

    LOG_DEBUG("init logging done");
}

} // namespace hera
