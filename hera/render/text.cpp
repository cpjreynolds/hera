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

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hera/input.hpp>
#include <hera/config.hpp>
#include <hera/utility.hpp>
#include <hera/render/text.hpp>

template<>
struct std::default_delete<FT_FaceRec> {
    constexpr default_delete() noexcept = default;
    constexpr void operator()(FT_Face ptr) const { FT_Done_Face(ptr); }
};

template<>
struct std::default_delete<std::remove_pointer_t<FT_Library>> {
    constexpr default_delete() noexcept = default;
    constexpr void operator()(FT_Library ptr) const { FT_Done_FreeType(ptr); }
};

inline static void fterr(FT_Error ec)
{
    if (ec != 0) {
        const char* estr = FT_Error_String(ec);
        LOG_CRITICAL("Freetype error: {}", estr);
        throw hera::runtime_error("Freetype error");
    }
}

namespace hera::render {
namespace {
struct Face {
    // FT_Face _face = nullptr;
    unique_ptr<FT_FaceRec> _face;

    Face() {};
    Face(FT_Face f) : _face{f} {}

    FT_Face operator->() const { return _face.get(); }

    // set char size in 1/64 points.
    void char_size(FT_F26Dot6 w, FT_F26Dot6 h) const
    {
        auto dpi = raw_input::dpi();
        fterr(FT_Set_Char_Size(_face.get(), w, h, dpi.x, dpi.y));
    }
    // set char size in physical pixels.
    void pixel_sizes(FT_UInt w, FT_UInt h) const
    {
        fterr(FT_Set_Pixel_Sizes(_face.get(), w, h));
    }

    // set char size in integer points
    void point_size(int w, int h) const { char_size(w << 6, h << 6); }

    void request_size(FT_F26Dot6 w, FT_F26Dot6 h, FT_Size_Request_Type ty) const
    {
        FT_Size_RequestRec req;
        auto dpi = raw_input::dpi();
        req.horiResolution = dpi.y;
        req.vertResolution = dpi.x;
        req.type = ty;
        req.height = h;
        req.width = w;
        fterr(FT_Request_Size(_face.get(), &req));
    }

    // glyph index of character code.
    FT_UInt char_index(FT_ULong ch) const
    {
        return FT_Get_Char_Index(_face.get(), ch);
    }
    // {charcode, index} of first character.
    pair<FT_ULong, FT_UInt> first_char() const
    {
        decltype(first_char()) ret;
        ret.first = FT_Get_First_Char(_face.get(), &ret.second);
        return ret;
    }
    // {charcode, index} of next character after given.
    pair<FT_ULong, FT_UInt> next_char(FT_ULong from) const
    {
        decltype(next_char(from)) ret;
        ret.first = FT_Get_Next_Char(_face.get(), from, &ret.second);
        return ret;
    }

    void load_char(FT_ULong ch, FT_Int32 flags = FT_LOAD_RENDER) const
    {
        fterr(FT_Load_Char(_face.get(), ch, flags));
    }

    const FT_Size_Metrics& metrics() const { return _face->size->metrics; }

    // {w,h} to fit any char (fixed width)
    ivec2 dims() const
    {
        return {max_advance(), (metrics().ascender - metrics().descender) >> 6};
    }

    // max advance in integer pixels.
    int max_advance() const { return metrics().max_advance >> 6; }
    // ascender in integer pixels.
    int ascender() const { return metrics().ascender >> 6; }
    // descender in integer pixels.
    int descender() const { return metrics().descender >> 6; }
};

struct FTLibrary {
    unique_ptr<std::remove_pointer_t<FT_Library>> _ftlib;

    FTLibrary()
    {
        FT_Library lib;
        fterr(FT_Init_FreeType(&lib));
        _ftlib.reset(lib);
    }

    Face new_face(const path& p) const
    {
        FT_Face f;
        fterr(FT_New_Face(_ftlib.get(), p.c_str(), 0, &f));
        return f;
    };
};
} // namespace

// returns val aligned to next multiple of 4
static constexpr int align4(int val)
{
    assert(val >= 0);
    return (val + 3) & ~0x03;
}

static string default_alphabet()
{
    string buf;
    for (unsigned char ch = 0; ch != 0xff; ++ch) {
        buf.push_back(ch);
    }
    return buf;
}

static void add_char(Alphabet& alpha, const FT_GlyphSlotRec& glyph)
{
    const auto& bmp = glyph.bitmap;
    ivec2 bearing{glyph.bitmap_left, glyph.bitmap_top};
    int pitch = bmp.pitch;
    ivec2 bsz{bmp.width, bmp.rows};

    const auto& extents = alpha.extents;
    const auto& padded = alpha.padded;

    assert(alpha.size.y >= bsz.y);
    assert(alpha.size.x >= bsz.x);
    // only using 1 byte per pixel.
    assert(pitch == bsz.x);

    int dst_offset = ((extents.x - bearing.y) * padded.x) + bearing.x;

    auto dst_idx = alpha.buffer.size();
    alpha.buffer.resize(dst_idx + (padded.x * padded.y));

    unsigned char* src = bmp.buffer;
    unsigned char* dst = &alpha.buffer[dst_idx] + dst_offset;

    for (int i = 0; i < bsz.y; ++i, src += pitch, dst += padded.x) {
        std::copy_n(src, pitch, dst);
    }
}

Alphabet::Alphabet(const Config& config)
{
    gl::checkerror();
    path fontpath = path(config["assets.path"]) /
                    path(config["assets.fonts.path"]) /
                    path(config["assets.fonts.regular"]);
    FTLibrary lib;
    Face face = lib.new_face(fontpath.native());
    face.char_size(16 * 64, 0);
    size = face.dims();
    extents = {face.ascender(), face.descender()};
    padded = {align4(size.x), size.y};
    auto letters = default_alphabet();
    buffer.reserve(letters.size() * padded.x * padded.y);

    int ch_idx = 0;
    for (char ch : letters) {
        face.load_char(ch, FT_LOAD_RENDER);
        add_char(*this, *face->glyph);
        charmap.emplace(ch, ch_idx);
        ++ch_idx;
    }
    texarray.bind(3);
    gl::TextureParams params;
    params.wrap_s = GL_CLAMP_TO_EDGE;
    params.wrap_t = GL_CLAMP_TO_EDGE;
    gl::checkerror();
    texarray.params(params);

    gl::checkerror();
    texarray.allocate(gl::internal_f::red, size.x, size.y, ch_idx, buffer);
    gl::checkerror();
}

} // namespace hera::render

namespace hera {

namespace {

struct char_vertex {
    float pos[2];
    float tex[2];
};

array<char_vertex, 6> make_quad(float w, float h)
{
    return {{{{w, h}, {1, 0}},
             {{w, 0}, {1, 1}},
             {{0, 0}, {0, 1}},
             {{0, h}, {0, 0}}}};
}

/*
 * (0,h)*------------*(w,h)
 *      |            |
 *      |            |
 *      |            |
 *      |            |
 *      |            |
 *      |            |
 *      |            |
 * (0,0)*------------*(w,0)
 */
/*
 * (0,1)*------------*(1,1)
 *      |            |
 *      |            |
 *      |            |
 *      |            |
 *      |            |
 *      |            |
 *      |            |
 * (0,0)*------------*(1,0)
 */

/*
static constexpr char_vertex vertices[] = {
    {{1, 1}, {1, 1}},
    {{1, 0}, {1, 0}},
    {{0, 0}, {0, 0}},
    {{0, 1}, {0, 1}},
};
*/

static constexpr unsigned char quad_indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

} // namespace

template<>
struct gl::vertex<char_vertex> : attributes<float[4]> {};

namespace render {

Scribe::Scribe(const Config& config) : vbuf{}, alphabet{config}
{
    gl::checkerror();
    LOG_DEBUG("init text projector");
    const auto quad = make_quad(alphabet.size.x, alphabet.size.y);
    vbuf.data(quad, quad_indices);
}

void Scribe::put(char ch, float x, float y, const gl::Pipeline& prog) const
{
    prog.uniform("glyph", 3);
    alphabet.bind(3);
    prog.uniform("origin", vec2{1 + x, 1 + y});
    prog.uniform("text_color", vec3{0.0});
    auto chidx = alphabet.charmap.at(ch);
    prog.uniform("chidx", chidx);
    vbuf.draw();
}

} // namespace render
} // namespace hera
