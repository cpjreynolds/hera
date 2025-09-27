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

#include <cstdlib>

#include <cpptrace/cpptrace.hpp>
#include <cpptrace/formatting.hpp>

#include <hera/common.hpp>
#include <hera/init.hpp>
#include <hera/error.hpp>

namespace hera {
namespace {
[[noreturn]] void terminate_handler();
}

void init::error()
{
    LOG_DEBUG("init error");
    std::set_terminate(&terminate_handler);
#if HERA_STACKTRACE
    LOG_DEBUG("terminate stacktrace enabled");
#else
    LOG_DEBUG("terminate stacktrace disabled");
#endif
    LOG_DEBUG("init error done");
}

namespace {

#if !HERA_STACKTRACE

[[noreturn]] void terminate_handler()
{
#if HERA_NICEABORT
    LOG_INFO("terminating with exit(0)");
    quill::flush();
    std::exit(1);
#else // !NICEABORT
    LOG_INFO("terminating with abort()");
    quill::flush();
    std::abort();
#endif
}

#else // !STACKTRACE
#include <unistd.h>

void replace_all(string&, string_view, string_view);
string fixup_trace(string&);
void fixup_nested_pair(string&, char, char, size_t startpos = 0,
                       ptrdiff_t mintrim = 4, string_view repl = "…");
void fixup_filepaths(string&);

[[noreturn]] void terminate_handler()
{
#if HERA_NICEABORT
    LOG_INFO("terminating with exit(0)");
#else
    LOG_INFO("terminating with abort()");
#endif
    bool istty = isatty(STDOUT_FILENO);
    ostringstream buf;

    using color_mode = cpptrace::formatter::color_mode;
    using path_mode = cpptrace::formatter::path_mode;
    using address_mode = cpptrace::formatter::address_mode;
    using symbol_mode = cpptrace::formatter::symbol_mode;
    using cpptrace::generate_trace;
    auto fmt = cpptrace::formatter{}
                   .colors(istty ? color_mode::always : color_mode::none)
                   .addresses(address_mode::none)
                   .paths(path_mode::basename)
                   .symbols(symbol_mode::pretty);

    try {
        auto ptr = std::current_exception();
        if (ptr == nullptr) {
            std::println(buf, "terminate called without an active exception");
            fmt.print(buf, generate_trace());
        }
        else {
            std::rethrow_exception(ptr);
        }
    }
    catch (cpptrace::exception& e) {
        std::println(buf,
                     "terminate called after throwing an instance of {}: {}",
                     cpptrace::demangle(typeid(e).name()), e.message());
        // auto roughtrace = e.trace().to_string(istty);
        // auto trace = fixup_trace(roughtrace);
        // buf.write(trace.c_str(), trace.size());
        fmt.print(buf, generate_trace(3));
    }
    catch (std::exception& e) {
        std::println(buf,
                     "terminate called after throwing an instance of {}: {}",
                     cpptrace::demangle(typeid(e).name()), e.what());
        fmt.print(buf, generate_trace(3));
    }
    catch (...) {
        std::println(buf, "terminate after throwing unknown exception");
        fmt.print(buf, generate_trace(3));
    }

    if (istty) {
        hera::global_log->flush_log();
        auto bufdata = buf.str();
        constexpr string_view sgr0 = "\033[0m\n";
        std::cout.write(sgr0.data(), sgr0.size());
        auto out = fixup_trace(bufdata);
        std::cout.write(out.data(), out.size());
        // std::cout.write(bufdata.data(), bufdata.size());
        std::cout.flush();
    }
    else {
        LOG_ERROR("{}", std::move(buf).str());
        hera::global_log->flush_log();
    }
#if HERA_NICEABORT
    std::exit(1);
#else
    std::abort();
#endif
}
void replace_all(string& str, string_view substr, string_view replacement)
{
    auto pos = 0uz;
    while ((pos = str.find(substr.data(), pos, substr.size())) != str.npos) {
        str.replace(pos, substr.size(), replacement.data(), replacement.size());
        pos += replacement.size();
    }
}

string fixup_trace(string& trace)
{
    istringstream input{trace};
    string buf;
    for (string line; std::getline(input, line);) {
        if (line.contains("sol::")) {
            fixup_nested_pair(line, '<', '>');
            fixup_nested_pair(line, '(', ')');
        }
        replace_all(line, "std::__1::", "");
        replace_all(line, "fmtquill::v11", "fmt");
        replace_all(line, "hera::", "");
        replace_all(line, "std::", "");
        replace_all(line, "protected_function_result", "pfr");
        fixup_filepaths(line);
        buf.append(line);
        buf.push_back('\n');
    }
    buf.push_back('\n');
    return buf;
}

void fixup_nested_pair(string& line, char och, char cch, size_t startpos,
                       ptrdiff_t mintrim, string_view repl)
{
    do {
        int level = 0;
        if (startpos >= line.size()) {
            break;
        }
        auto start = line.end();
        auto stop = line.end();
        auto ch = line.begin();
        std::advance(ch, startpos);
        for (; ch != line.end(); ++ch) {
            if (*ch == och) {
                if (level == 0) {
                    start = ch + 1;
                }
                ++level;
            }
            else if (*ch == cch) {
                --level;
                if (level == 0) {
                    stop = ch;
                    break;
                }
            }
        }
        auto trimlen = std::distance(start, stop);
        auto enddist = std::distance(line.begin(), stop);

        if (trimlen != 0) {
            if (trimlen > mintrim) {
                line.replace(start, stop, repl);
                startpos = enddist - trimlen + repl.size() + 1;
            }
            else {
                startpos = enddist + 1;
            }
        }
        else {
            startpos = 0;
        }
    } while (startpos != 0);
}

void fixup_filepaths(string& line)
{
    auto startpos = line.find('/');
    if (startpos == line.npos)
        return;
    auto endpos = line.rfind('/');
    if (endpos != startpos) {
        endpos = line.rfind('/', endpos - 1);
    }
    else {
        return;
    }
    auto trimlen = endpos - startpos + 1;
    line.erase(startpos, trimlen);
}

} // namespace
} // namespace hera

#endif
