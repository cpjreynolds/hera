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

#ifndef HERA_TICK_HPP
#define HERA_TICK_HPP

#include <hera/common.hpp>

namespace hera {

inline constexpr intmax_t tick_hz = 120;

using ticks = duration<float, ratio<1, tick_hz>>;
using tick_count = duration<clock::duration::rep, ratio<1, tick_hz>>;

template<typename Period = ratio<1>>
consteval float tickrate()
{
    return duration<float, Period>{ticks{1}}.count();
}

struct Ticker {
    using time_point = clock::time_point;

    time_point prev = clock::now();
    tick_count total = tick_count::zero();
    ticks acc = ticks::zero();

    // add time since last push to accumulator.
    void push();

    // pop one tick from the accumulator.
    //
    // returns true while there are whole ticks left to pop.
    bool pop();

    // returns the tick fraction held in accumulator.
    float delta() const
    {
        assert(acc < ticks{1});
        return acc.count();
    }
};

} // namespace hera

#endif
