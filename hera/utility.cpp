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
#include <filesystem>
#if defined(UNIX)
#include <unistd.h>
#include <pwd.h>
#endif

#include <cstdlib>

#include <whereami.h>

#include <hera/common.hpp>
#include <hera/utility.hpp>

namespace hera {

string slurp(const char* fname)
{
    ifstream fs(fname);
    ostringstream buf;
    buf << fs.rdbuf();
    return std::move(buf).str();
}

string slurp(const string& fname)
{
    return slurp(fname.c_str());
}

/*
u32string u32conv(string_view from_str)
{
    using enum std::codecvt_base::result;
    auto loc = std::locale();
    auto& facet =
        std::use_facet<std::codecvt<char32_t, char8_t, mbstate_t>>(loc);

    mbstate_t state{};
    const char8_t* from = reinterpret_cast<const char8_t*>(from_str.data());
    const char8_t* from_end = from + from_str.size();
    const char8_t* from_next;

    u32string tostr(from_str.size(), '\0');
    char32_t* to = tostr.data();
    char32_t* to_end = to + tostr.size();
    char32_t* to_next;

    auto rc = facet.in(state, from, from_end, from_next, to, to_end, to_next);
    tostr.resize(to_next - to);

    switch (rc) {
    case partial:
        throw hera::range_error("partial UTF conversion");
    case error:
        throw hera::range_error("UTF conversion error");
    case noconv:
        throw hera::range_error("unexpected UTF noconv");
    case ok:
        return tostr;
    }
}
*/

path get_exe_path()
{
    int len = wai_getExecutablePath(nullptr, 0, nullptr);
    string pat(len + 1, '\0');
    wai_getExecutablePath(pat.data(), len, nullptr);
    return pat;
}

path get_exe_dir()
{
    auto p = get_exe_path();
    return p.parent_path();
}

path get_home_dir()
{
    path ret;
#if defined(WINDOWS)
    const char* upro = std::getenv("USERPROFILE");
    ret = upro ? upro : "";
    if (ret.empty() || !fs::exists(ret)) {
        const char* hdrive = std::getenv("HOMEDRIVE");
        hdrive = hdrive ? hdrive : "";
        const char* hpath = std::getenv("HOMEPATH");
        hpath = hpath ? hpath : "";
        ret = hdrive / hpath;
        if (ret.empty() || !fs::exists(ret)) {
            const char* home = std::getenv("HOME");
            home = home ? home : "";
            ret = home;
        }
    }
#elif defined(UNIX)
    const char* home = std::getenv("HOME");
    ret = home ? home : "";
    if (ret.empty() || !fs::exists(ret)) {
        struct passwd* pwd = getpwuid(getuid());
        if (pwd && pwd->pw_dir) {
            ret = pwd->pw_dir;
        }
    }
#endif
    if (ret.empty() || !fs::exists(ret)) {
        ret = fs::current_path().root_path();
    }
    return ret;
}

path get_local_dir()
{
    auto ret = get_home_dir();
#if defined(WINDOWS)
    ret /= "Documents/hera";
#else
    ret /= ".config/hera";
#endif
    if (!fs::exists(ret)) {
        fs::create_directories(ret);
    }
    return ret;
}

} // namespace hera
