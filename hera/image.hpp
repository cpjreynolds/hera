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

namespace hera {

using image_load_t =
    tuple<unique_ptr<unsigned char, decltype(&free)>, ivec2, int>;

// loads an image file.
//
// returns {buffer, size, channels}
image_load_t load_image(const path&);

image_load_t load_image(const unsigned char* buf, int len);

template<spanner R>
    requires same_as<range_v<R>, unsigned char>
image_load_t load_image(const R& buf)
{
    load_image(ranges::cdata(buf), ranges::size(buf));
}

} // namespace hera
