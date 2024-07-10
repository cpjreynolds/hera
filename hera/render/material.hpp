#ifndef HERA_RENDER_MATERIAL_HPP
#define HERA_RENDER_MATERIAL_HPP

#include <hera/image.hpp>
#include <hera/gl/texture.hpp>

namespace hera {

struct Material {
    gl::Texture2d diff;
    gl::Texture2d spec;
};

} // namespace hera

#endif
