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
#include <hera/common.hpp>
#include <hera/utility.hpp>
#include <hera/error.hpp>
#include <hera/log.hpp>
#include <hera/init.hpp>
#include <hera/toml.hpp>
#include <hera/config.hpp>

#include <hera/lua.hpp>

namespace hera {

namespace {
static constexpr string_view CONFNAME = "hera.toml";

path root_config_path()
{
    constexpr string_view searchdirs[] = {"config", "Resources/config",
                                          "share/hera/config"};

    for (auto curpath = hera::get_exe_dir(); curpath.has_filename();
         curpath = curpath.parent_path()) {
        LOG_TRACE_L1("searching for config in: {}", curpath);
        if (fs::exists(curpath / CONFNAME)) {
            return curpath / CONFNAME;
        }
        for (const auto& sdir : searchdirs) {
            auto maybepath = curpath / sdir / CONFNAME;
            LOG_TRACE_L1("searching for config in: {}", curpath / sdir);
            if (fs::exists(maybepath)) {
                return maybepath;
            }
        }
    }
    return {};
}

toml::table configurate()
{
    auto rootpath = root_config_path();
    if (rootpath.empty()) {
        LOG_CRITICAL("root config not found");
        throw hera::runtime_error("could not find root config");
    }
    LOG_INFO("root config: {}", rootpath);
    // filepaths are relative to root config file so set cwd accordingly
    fs::current_path(rootpath.parent_path());

    auto table = toml::parse_file(rootpath.native());

    auto userpath = hera::get_local_dir() / CONFNAME;
    if (fs::exists(userpath)) {
        LOG_INFO("user config: {}", userpath);
        auto usertbl = toml::parse_file(userpath.native());
        table = toml::inherit(table, usertbl);
    }
    return table;
}
} // namespace

Config::table_t* Config::global_table = nullptr;

void init::config()
{
    LOG_DEBUG("init config");
    static toml::table tbl = configurate();
    Config::global_table = &tbl;
    auto loglevel_cfg = tbl.at_path("logging.level").value_or<string>("debug");
    auto loglevel = quill::loglevel_from_string(loglevel_cfg);
    auto logstr_proper = quill::loglevel_to_string(loglevel);
    LOG_INFO("log level: {}", logstr_proper);
    quill::get_logger()->set_log_level(loglevel);
    LOG_DEBUG("init config done");
}

} // namespace hera
