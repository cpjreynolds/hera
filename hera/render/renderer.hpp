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

#ifndef HERA_RENDER_RENDERER_HPP
#define HERA_RENDER_RENDERER_HPP

#include <glm/trigonometric.hpp>

#include <hera/common.hpp>
#include <hera/config.hpp>
#include <hera/input.hpp>
#include <hera/gl/program.hpp>
#include <hera/render/camera.hpp>
#include <hera/render/text.hpp>

namespace hera {

template<typename T>
concept drawable =
    requires(T& obj, const gl::Pipeline& s, float a) { obj.draw(s, a); };

class Renderer {
    GLFWwindow* _window;

public:
    Scribe projector;
    gl::Shaders shaders;

    Renderer(const Config&);
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    ~Renderer() { input::actions.disconnect(this); }

    void init(const Config&);

    template<drawable T>
    void draw(T& obj, float alpha = 1.0)
    {
        obj.draw(shaders.active(), alpha);
    }

    const gl::Pipeline& pipeline(string_view name);
    const gl::Pipeline& pipeline() const;

    class Frame {
        Renderer* rdr{nullptr};

        Frame() {};
        Frame(Renderer* r) : rdr{r} { rdr->begin_frame(); }

    public:
        Frame(const Frame&) = delete;
        Frame& operator=(const Frame&) = delete;
        Frame(Frame&& r) : rdr{std::exchange(r.rdr, nullptr)} {}
        Frame& operator=(Frame&& r)
        {
            Frame(std::move(r)).swap(*this);
            return *this;
        }
        void swap(Frame& other) { std::swap(rdr, other.rdr); }

        Frame& operator++()
        {
            rdr->swap();
            rdr = nullptr;
            return *this;
        }

        Renderer& operator*() { return *rdr; }
        Renderer* operator->() { return rdr; }

        friend bool operator==(const Frame& l, std::default_sentinel_t)
        {
            return l.rdr == nullptr;
        }
        friend class Renderer;
        using difference_type = ptrdiff_t;
        using value_type = Renderer;
        using reference = Renderer&;
        using pointer = Renderer*;
        using iterator_category = std::input_iterator_tag;
    };

    Frame begin() { return Frame(this); }
    std::default_sentinel_t end() { return std::default_sentinel; }

    void begin_frame();
    void swap() { glfwSwapBuffers(_window); }

    void on_action(input_action);

private:
    friend struct fmt::formatter<Renderer>;
};

} // namespace hera

template<>
struct fmt::formatter<hera::Renderer> : hera::format_parser<> {
    auto format(const hera::Renderer& val, auto& ctx) const
    {
        auto output = ctx.out();
        fmt::format_to(output, "Renderer:\n{}", val.shaders);
        return output;
    }
};

#endif
