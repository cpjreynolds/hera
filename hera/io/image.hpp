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

#ifndef HERA_IMAGE_HPP
#define HERA_IMAGE_HPP

#include <hera/common.hpp>
#include <hera/io/assets.hpp>

namespace hera {

struct image_data {
    struct deleter {
        void operator()(uint8_t* p) const { ::free(p); }
    };
    unique_ptr<uint8_t, deleter> buf;
    ivec2 size;
    int channels;

    size_t size_bytes() const { return 1uz * size.x * size.y * channels; }

    span<uint8_t> operator*() const { return span{buf.get(), size_bytes()}; }
};

template<>
struct asset<image_data> {
    shared_ptr<image_data> load_from(const link& p);
};

}; // namespace hera

#endif
