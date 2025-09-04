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

#ifndef HERA_PROGRAMS_HPP
#define HERA_PROGRAMS_HPP

#include <hera/common.hpp>
#include <hera/log.hpp>
#include <hera/format.hpp>
#include <hera/gl/common.hpp>
#include <hera/gl/object.hpp>

namespace hera::gl {

class Shader : private object<id::program(1)> {
    inline static hash_set<string> _missing;
    static constexpr id::program pID{0};
    path _fpath;
    string _fname;
    shader_t _type;

    // name -> {loc, type, size}
    mutable hash_map<string, tuple<GLint, GLenum, GLint>> _uniforms{};
    // name -> {idx}
    mutable hash_set<string> _blocks;

    explicit Shader(nullptr_t) : object{nullptr} {}

public:
    explicit Shader(const path& pat);

    static const Shader null;

    constexpr id::program id() const { return get<pID>(); }
    shader_t type() const { return _type; }
    string_view filename() const { return _fname; }
    string_view modname() const;

    int query(GLenum q) const { return gl::parameter(id(), q); }
    // compile and link
    void load() const;
    string info_log() const;

    bool is_null() const { return id() == 0; }
    explicit operator bool() const { return !is_null(); }

    template<uniformable T>
    optional<int> has_uniform(string_view name, const T& v) const
    {
        using value_type = uniform_traits<T>::value_type;
        if (auto it = _uniforms.find(name); it != _uniforms.cend()) {
            auto&& [loc, type, size] = it->second;
            if (compatible_uniform(type, gl_typeof<value_type>()) &&
                size == uniform_size(v) && loc != -1) {
                return loc;
            }
        }
        return nullopt;
    }

    template<uniformable T>
    void uniform(string_view name, const T& v) const
    {
        if (auto loc = has_uniform<T>(name, v); loc) {
            gl::uniform(id(), *loc, v);
        }
        else {
            auto&& [_, first_miss] = _missing.emplace(
                fmt::format("{} {}:{}", type_of<T>(), filename(), name));
            if (first_miss) {
                LOG_ERROR("uniform doesn't exist: {} {}:{}", type_of<T>(),
                          filename(), name);
            }
        }
    }

    template<uniformable T>
    void uniform(int loc, const T& v) const
    {
        gl::uniform(id(), loc, v);
    }

    friend void swap(Shader& l, Shader& r) { l.swap(r); }
    void swap(Shader& other)
    {
        object::swap(other);
        std::swap(_fpath, other._fpath);
        std::swap(_fname, other._fname);
        std::swap(_type, other._type);
    }

private:
    friend class Shaders;

    struct block_info {
        int size;
        vector<GLenum> types;

        friend auto operator<=>(const block_info&, const block_info&) = default;
    };

    vector<pair<string, block_info>> build_cache() const;

    static constexpr bool compatible_uniform(GLenum expect, GLenum have)
    {
        bool sampler = (expect == GL_SAMPLER_1D || expect == GL_SAMPLER_2D ||
                        expect == GL_SAMPLER_2D_ARRAY);
        return (expect == have) || (sampler && have == GL_INT);
    }
};

class Pipeline : private object<id::pipeline(1)> {
    static constexpr id::pipeline pipeID{0};
    string _name;
    Shader _vert = Shader::null;
    Shader _frag = Shader::null;

public:
    explicit Pipeline(nullptr_t) : object{nullptr}, _name{"null"} {};
    explicit Pipeline(string_view name) : _name{name} {};
    Pipeline(string_view name, const Shader& v, const Shader& f)
        : _name{name},
          _vert{v},
          _frag{f} {};

    static const Pipeline null;

    constexpr id::pipeline id() const { return object::get<pipeID>(); }

    void bind() const { gl::bind(id()); }
    void attach(const Shader& s);

    string info_log() const;
    bool valid() const;
    string_view name() const { return _name; }

    template<uniformable T>
    void uniform(string_view name, const T& v) const
    {
        if (auto loc = _vert.has_uniform<T>(name, v); loc) {
            _vert.uniform(*loc, v);
        }
        if (auto loc = _frag.has_uniform<T>(name, v); loc) {
            _frag.uniform(*loc, v);
        }
    }

    decltype(auto) vert(this auto& self) { return self._vert; }
    decltype(auto) frag(this auto& self) { return self._frag; }

    template<size_t I>
    decltype(auto) get(this auto& self)
    {
        if constexpr (I == 0) {
            return self._vert;
        }
        else if constexpr (I == 1) {
            return self._frag;
        }
        else {
            throw hera::runtime_error("bad index");
        }
    }
};

namespace detail {

template<typename T>
consteval size_t align_offset(size_t n)
{
    constexpr size_t align = gl_alignof<T>();
    return ((n + (align - 1)) / align) * align;
}

// glsl aligned offset of the nth member.
template<size_t I, typename... Ts>
static constexpr size_t aligned_offset =
    size_constant<align_offset<tuple_element_t<I, tuple<Ts...>>>(
        sizeof(tuple_element_t<I - 1, tuple<Ts...>>) +
        aligned_offset<I - 1, Ts...>)>::value;

template<typename... Ts>
static constexpr size_t aligned_offset<0, Ts...> = size_constant<0>::value;

template<glsl_type T>
struct payload_of_t;

template<gl_scalar T>
struct payload_of_t<T> {
    using type = T;
};

template<gl_matrix T>
struct payload_of_t<T> : mat<gl_cols<T>(), 4, typename T::value_type> {};

template<gl_vector T>
struct payload_of_t<T> : std::conditional<gl_length(T{}) == 3, vec4, T> {};

// provides unique block binding points for each buffer.
static GLuint next_block_binding = 0;
} // namespace detail

template<glsl_type... Ts>
class UniformBuffer : object<id::buffer{1}> {
    static constexpr id::buffer uboID{0};
    using buffer_t::uniform;
    friend class UniformBuffer<>;

    // the block index of this buffer.
    GLuint _block_binding;
    // global name (in all shaders) of the uniform block.
    string _name;

    template<size_t I>
    static constexpr size_t offset_at = detail::aligned_offset<I, Ts...>;

    template<glsl_type T>
    using payload_of = detail::payload_of_t<T>::type;

    template<size_t I>
    using element_at = std::tuple_element_t<I, UniformBuffer>;

public:
    constexpr id::buffer id() const { return get<uboID>(); }
    static consteval size_t size() { return sizeof...(Ts); }
    static consteval size_t size_bytes()
    {
        return sizeof(tuple<gl_aligned<Ts>...>);
    }

    UniformBuffer(string_view name)
        : _block_binding{detail::next_block_binding++},
          _name{name}
    {
        gl::bind(id(), uniform);
        // allocate the buffer.
        gl::allocate(uniform, size_bytes(), buffer_use::static_draw);
        gl::bind(id(), uniform, _block_binding);
    }

    string_view name() const { return _name; }

    // binds the uniform buffer to the generic bind point.
    void bind() const { gl::bind(id(), uniform); }

    void bind_to(const Shader& prog) const
    {
        auto idx = glGetUniformBlockIndex(prog.id(), _name.c_str());
        glUniformBlockBinding(prog.id(), idx, _block_binding);
    }

    // returns the block index for this buffer
    GLuint binding() const { return _block_binding; }

    template<size_t I>
    void write(const element_at<I>& val)
    {
        using elt_t = element_at<I>;

        bind();
        if constexpr (same_as<elt_t, payload_of<elt_t>>) {
            static_assert(sizeof(val) == gl_size<elt_t>());
            gl::data(uniform, offset_at<I>, views::single(val));
        }
        else if constexpr (same_as<elt_t, vec3>) {
            payload_of<elt_t> payload = glm::make_vec4(val);
            static_assert(sizeof(payload) == gl_size<elt_t>());
            gl::data(uniform, offset_at<I>, views::single(payload));
        }
        else {
            payload_of<elt_t> payload{val};
            static_assert(sizeof(payload) == gl_size<elt_t>());
            gl::data(uniform, offset_at<I>, views::single(payload));
        }
    }
};

// type-erased uniform buffer.
template<>
class UniformBuffer<> : object<id::buffer{1}> {
    static constexpr id::buffer uboID{0};
    using buffer_t::uniform;

    // the block index of this buffer.
    GLuint _block_binding;
    // global name (in all shaders) of the uniform block.
    string _name;

public:
    UniformBuffer() = delete;

    template<glsl_type... Ts>
    UniformBuffer(const UniformBuffer<Ts...>& concrete)
        : object{concrete},
          _block_binding{concrete._block_binding},
          _name{concrete._name} {};

    constexpr id::buffer id() const { return get<uboID>(); }

    string_view name() const { return _name; }

    // binds the uniform buffer to the generic bind point.
    void bind() const { gl::bind(id(), uniform); }

    void bind_to(const Shader& prog) const
    {
        auto idx = glGetUniformBlockIndex(prog.id(), _name.c_str());
        glUniformBlockBinding(prog.id(), idx, _block_binding);
    }

    GLuint binding() const { return _block_binding; }
};

namespace detail {
template<typename T>
concept has_modname = requires(T t) {
    { t.modname() } -> std::convertible_to<string_view>;
};

template<typename T>
concept module_key = has_modname<T> || std::convertible_to<T, string_view>;

template<glsl_type T>
bool type_eq(GLenum other)
{
    return gl_typeof<T>() == other;
}

} // namespace detail

class Shaders {
    hash_map<string, Shader::block_info> _block_infos;
    hash_map<string, UniformBuffer<>> _blocks;
    hash_map<string, Shader> _shaders;
    hash_map<string, Pipeline> _pipelines;

    const Pipeline* _active = &Pipeline::null;

    using shaders_t = decltype(_shaders);
    using blocks_t = decltype(_blocks);
    using pipelines_t = decltype(_pipelines);

    friend struct fmt::formatter<Shaders>;

public:
    Shaders() {};
    Shaders(const Shaders&) = delete;
    Shaders& operator=(const Shaders&) = delete;

    auto operator->(this auto& self) { return self._active; }
    decltype(auto) operator*(this auto& self) { return *self._active; }

    // Loads all shaders in a directory.
    //
    // shader files which share a filename stem are considered a "module" and
    // will be linked together into a pipeline of the same name.
    void load(const path&);

    // links the shaders of module `name` into a program.
    void link(string_view name);

    template<glsl_type... Ts>
    void add_block(const UniformBuffer<Ts...>& buf)
    {
        auto binfo = _block_infos.find(buf.name());
        if (binfo == _block_infos.end()) {
            LOG_CRITICAL("uniform block missing: {}", buf.name());
            throw hera::runtime_error("missing uniform block");
        }
        auto&& [size, types] = binfo->second;
        if (types.size() != sizeof...(Ts)) {
            LOG_CRITICAL(
                "uniform block length mismatch: {}: expect: {}, got: {}",
                buf.name(), types.size(), sizeof...(Ts));
            throw hera::runtime_error("mismatched uniform block length");
        }
        auto ty_iter = types.begin();
        bool is_match = (detail::type_eq<Ts>(*ty_iter++) && ...);
        if (!is_match) {
            GLenum given_types[sizeof...(Ts)] = {gl_typeof<Ts>()...};
            auto badidx = std::distance(types.begin(), ty_iter - 1);

            auto bad_ty = given_types[badidx];
            auto expect_ty = types[badidx];

            LOG_CRITICAL("uniform block type mismatch: expect: {}, got: {}",
                         expect_ty, bad_ty);
            throw hera::runtime_error("uniform block type mismatch");
        }
        const auto& blk =
            _blocks.emplace(buf.name(), UniformBuffer<>(buf)).first;
        bind_block(blk->second);
    }

    // returns the active pipeline.
    const Pipeline& active() const;
    // sets the active pipeline and returns it.
    const Pipeline& active(string_view name);

private:
    // binds a uniform buffer to all programs that want it.
    void bind_block(const UniformBuffer<>&) const;
    // binds all contained uniform buffers to a given program.
    void bind_blocks_to(const Shader&) const;
    // loads and compiles a single shader file
    const Shader& loadf(const path&);
};
} // namespace hera::gl

template<size_t I, typename... Ts>
struct std::tuple_element<I, hera::gl::UniformBuffer<Ts...>>
    : std::tuple_element<I, std::tuple<Ts...>> {};

template<typename... Ts>
struct std::tuple_size<hera::gl::UniformBuffer<Ts...>>
    : std::integral_constant<size_t, sizeof...(Ts)> {};

template<size_t I>
struct std::tuple_element<I, hera::gl::Pipeline> {
    using type = hera::gl::Shader;
};

template<>
struct std::tuple_size<hera::gl::Pipeline> : std::integral_constant<size_t, 2> {
};

template<>
struct fmt::formatter<hera::gl::Shader> : hera::format_parser<> {
    auto format(const hera::gl::Shader& val, auto& ctx) const
    {
        auto output = ctx.out();
        if (alternate) {
            return fmt::format_to(output, "{}({})", val.filename(), val.id());
        }
        else {
            return fmt::format_to(output, "{}", val.filename());
        }
    }
};

template<>
struct fmt::formatter<hera::gl::Pipeline> : hera::format_parser<> {
    auto format(const hera::gl::Pipeline& val, auto& ctx) const
    {
        auto output = ctx.out();
        if (alternate) {
            return fmt::format_to(output, "{}[{:#}, {:#}]", val.name(),
                                  val.vert(), val.frag());
        }
        else {
            return fmt::format_to(output, "{}", val.name());
        }
    }
};

template<>
struct fmt::formatter<hera::gl::Shaders> : hera::format_parser<> {
    auto format(const hera::gl::Shaders& val, auto& ctx) const
    {
        auto output = ctx.out();

        auto pipes = val._pipelines | std::views::values;
        if (alternate) {
            fmt::format_to(output, "pipelines: {::#}", pipes);
        }
        else {
            fmt::format_to(output, "pipelines: {}", pipes);
        }
        for (auto&& [bname, binfo] : val._block_infos) {
            fmt::format_to(output, "\nblock: {} {{ ", bname);
            for (auto& ty : binfo.types) {
                fmt::format_to(output, "{} ", hera::gl::gl_str(ty));
            }
            fmt::format_to(output, "}} ({}b)", binfo.size);
        }

        return output;
    }
};

#endif
