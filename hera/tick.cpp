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

#include <hera/tick.hpp>

namespace hera {

namespace {

// apple OpenGL has a bugged vsync implementation.
// This is the current workaround.
void busy_sleep(const Ticker::time_point& prev)
{
    using namespace std::chrono_literals;
    auto since = [&] { return clock::now() - prev; };
    for (ticks diff = since(); diff < ticks{1}; diff = since()) {
        auto timeleft = ticks{1} - diff;
        if (timeleft >= 1ms) {
            std::this_thread::sleep_for(timeleft - 900us);
        }
        else {
            std::this_thread::yield();
        }
    }
}

} // namespace

void Ticker::push()
{
#if HERA_APPLE_FUCKERY
    busy_sleep(prev);
#endif
    auto now_time = clock::now();
    acc += now_time - prev;
    prev = now_time;
}

bool Ticker::pop()
{
    if (acc >= ticks{1}) {
        --acc;
        total++;
        return true;
    }
    else {
        return false;
    }
}

} // namespace hera
