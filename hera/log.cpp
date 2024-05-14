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

namespace hera {

void init::logging()
{
    const string pattern =
        "[%(time)][%(thread_name:^6)][%(log_level:^8)] - %(message)";
    const string timepat = "%H:%M:%S.%Qms";

    quill::Config cfg;

    if (isatty(STDOUT_FILENO)) {
        auto out_logger = quill::stdout_handler();
        out_logger->set_pattern(pattern, timepat);
        static_cast<quill::ConsoleHandler*>(out_logger.get())
            ->enable_console_colours();
        cfg.default_handlers.push_back(out_logger);
    }
    else {
        auto logdir = hera::get_local_dir() / "log";
        fs::create_directories(logdir);
        auto logfile = logdir / "hera.log";
        quill::FileHandlerConfig fileconf;
        fileconf.set_append_to_filename(quill::FilenameAppend::StartDateTime);
        auto file_logger = quill::file_handler(logfile, fileconf);
        file_logger->set_pattern(pattern, timepat);
        cfg.default_handlers.push_back(file_logger);
    }

    quill::configure(cfg);
    quill::start();
    quill::detail::set_thread_name("main");
    auto logger = quill::get_logger();
    logger->set_log_level(LogLevel::Debug);
    LOG_DEBUG("init logging done");
}

} // namespace hera
