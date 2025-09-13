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

#ifndef HERA_FORMAT_HPP
#define HERA_FORMAT_HPP

#include <hera/common.hpp>

namespace hera {

struct common_format_parser {
    bool alternate = false;

    template<typename ParseCtx>
    constexpr ParseCtx::iterator parse(ParseCtx& ctx)
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it == end) {
            return it;
        }
        for (; it != end && *it != ':' && *it != '}'; ++it) {
            switch (*it) {
            case '#':
                alternate = true;
                break;
            default:
                throw fmt::format_error("bad parse args");
            }
        }
        return it;
    }
};

template<typename T = void>
struct format_parser : common_format_parser, fmt::formatter<T> {
    template<typename ParseCtx>
    constexpr ParseCtx::iterator parse(ParseCtx& ctx)
    {
        auto it = common_format_parser::parse(ctx);
        ctx.advance_to(it);
        return fmt::formatter<T>::parse(ctx);
    }
};

template<>
struct format_parser<void> : common_format_parser {};

};

#endif
