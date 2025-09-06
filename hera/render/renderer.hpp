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

namespace hera {

class Renderer {
private:
    GLFWwindow* _window;

    struct Private {
        explicit Private() = default;
    };

public:
    Renderer(const Config&, Private);

    gl::Shaders shaders;

    static shared_ptr<Renderer> create(const Config& cfg)
    {
        auto self = std::make_shared<Renderer>(cfg, Private{});
        input::actions.connect<&Renderer::on_action>(self);
        return self;
    }

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    const gl::Pipeline& pipeline(string_view name);
    const gl::Pipeline& pipeline() const;

    class Frame {
    private:
        Renderer& rdr;
        friend class Renderer;

    public:
        Frame(Renderer& r) : rdr{r} { rdr.begin_frame(); }
        Frame(const Frame&) = delete;
        Frame& operator=(const Frame&) = delete;
        Frame(Frame&&) = delete;
        Frame& operator=(Frame&&) = delete;

        ~Frame() { rdr.swap(); }

        Renderer& operator*() { return rdr; }
        Renderer* operator->() { return &rdr; }
    };

    void swap() { glfwSwapBuffers(_window); }

    void on_action(input_action);

private:
    void begin_frame();
    friend struct fmt::formatter<Renderer>;
};

using Frame = Renderer::Frame;

class Drawable {
public:
    virtual ~Drawable() = default;

    virtual void draw(Frame& f, float alpha) const = 0;
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
