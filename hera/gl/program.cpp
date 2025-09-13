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

#include <charconv>

#include <hera/log.hpp>
#include <hera/gl/program.hpp>

namespace hera::gl {

// =====[Preprocessor]=====

void Preprocessor::define(const string& key, const string& value)
{
    _defines.insert_or_assign(key, value);
}

string Preprocessor::preprocess(const string& src,
                                const defines_map& local_defines) const
{
    string buf;
    auto output = back_inserter(buf);
    for (auto&& [k, v] : _defines) {
        std::format_to(output, "#define {} {}\n", k, v);
    }
    for (auto&& [k, v] : local_defines) {
        std::format_to(output, "#define {} {}\n", k, v);
    }

    buf.append(src);
    return buf;
}

// =====[Shader]=====

namespace {
// raii shader object handle
// just to ensure cleanup on exception
struct shader_handle {
    GLuint id;

    shader_handle(shader_t type) : id{glCreateShader(+type)} {}
    ~shader_handle() { glDeleteShader(id); }
};
} // namespace

const Shader Shader::null{nullptr};

string Shader::make_modname(const path& fpath)
{
    string rv = fpath.native();
    auto loc = rv.rfind('.');
    rv.erase(loc, string::npos);
    return rv;
}

shader_t Shader::classify(const path& fpath)
{
    auto ext = fpath.extension();
    if (ext == ".vert") {
        return shader_t::vertex;
    }
    else if (ext == ".frag") {
        return shader_t::fragment;
    }
    else {
        return {};
    }
}

Shader::Shader(path pat)
    : _fpath{std::move(pat)},
      _fname{_fpath.filename()},
      _type{classify(_fpath)},
      _modname{make_modname(_fname)}
{
    if (!_type.valid()) {
        LOG_ERROR("indeterminate shader type: {}", _fpath.filename());
        throw gl_error("indeterminate shader type");
    }
    gl::parameter(id(), GL_PROGRAM_SEPARABLE, true);
}

void Shader::update_link_log() const
{
    _link_log.clear();
    if (auto len = query(GL_INFO_LOG_LENGTH); len != 0) {
        // len includes null terminator, which string already handles
        _link_log.resize(len - 1);
        glGetProgramInfoLog(id(), len, nullptr, _link_log.data());
    }
}

void Shader::update_compilation_log(GLuint sID) const
{
    _compilation_log.clear();
    int len;
    glGetShaderiv(sID, GL_INFO_LOG_LENGTH, &len);
    if (len == 0) {
        return;
    }
    string newinfo;
    newinfo.resize(len - 1);
    glGetShaderInfoLog(sID, len, nullptr, newinfo.data());

    istringstream iss{std::move(newinfo)};
    for (string line; std::getline(iss, line);) {
        auto idx1 = line.find(':', 0);
        if (idx1 == line.npos) {
            continue;
        }
        auto idx2 = line.find(':', idx1 + 1);
        if (idx2 == line.npos) {
            continue;
        }
        idx1 += 2;
        size_t fnum;
        std::from_chars(&line[idx1], &line[idx2], fnum);
        line.replace(idx1, idx2 - idx1, _fname_indices.at(fnum));
        _compilation_log.append(line);
        _compilation_log.push_back('\n');
    }
    // remove the last newline.
    _compilation_log.pop_back();
}

void Shader::read()
{
    _source = slurp(_fpath);
}

void Shader::index_fnames()
{
    // filename to index
    // (doesnt need to own the key string)
    unordered_map<string, string> fn_num;

    istringstream iss{std::move(_source)};
    _source.clear();
    for (string line; std::getline(iss, line);) {
        if (line.starts_with("#extension GL_GOOGLE")) {
            continue;
        }
        if (line.starts_with("#line")) {
            size_t qi = line.find('"');
            size_t ei = line.size() - qi;
            auto fname = line.substr(qi + 1, ei - 2);
            auto fnum = std::to_string(fn_num.size());
            auto [elt, is_uniq] = fn_num.insert({fname, fnum});
            if (is_uniq) {
                // new filename encountered
                _fname_indices.push_back(fname);
            }
            else {
                // not a new file. use its existing number.
                fnum = elt->second;
            }
            line.replace(qi, ei, fnum);
        }
        _source.append(line);
        _source.push_back('\n');
    }
    if (_fname_indices.empty()) {
        _fname_indices.push_back(_fname.native());
    }
}

void Shader::load(const Shaders& ctx)
{
    if (!_uniforms.empty()) {
        LOG_INFO("reloading shader: {}", _fname);
        _uniforms.clear();
        _blocks.clear();
        _fname_indices.clear();
    }
    else {
        LOG_INFO("loading shader: {}", _fname);
    }
    read();
    preprocess(ctx.preprocessor());
    compile();
}

void Shader::define(const string& key, const string& value)
{
    _defines.insert_or_assign(key, value);
}

void Shader::preprocess(const Preprocessor& preproc)
{
    _source = preproc.preprocess(_source, _defines);
    index_fnames();
}

void Shader::compile() const
{
    shader_handle s{_type};

    auto data = _source.data();
    glShaderSource(s.id, 1, &data, nullptr);
    glCompileShader(s.id);
    update_compilation_log(s.id);

    int param;
    glGetShaderiv(s.id, GL_COMPILE_STATUS, &param);
    if (!param) {
        LOG_ERROR("shader compile error: {}", _compilation_log);
        throw gl_error("shader compile error");
    }

    glAttachShader(id(), s.id);
    glLinkProgram(id());
    glDetachShader(id(), s.id);
    update_link_log();

    // error checking
    if (!query(GL_LINK_STATUS)) {
        LOG_ERROR("shader link error: {}", _link_log);
        throw gl_error("shader link error");
    }
}

vector<pair<string, Shader::block_info>> Shader::build_cache() const
{
    LOG_TRACE_L1("building shader cache: {}", filename());
    int n_blocks = query(GL_ACTIVE_UNIFORM_BLOCKS);
    int n_uniforms = query(GL_ACTIVE_UNIFORMS);
    int blk_maxlen = query(GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH);
    int uni_maxlen = query(GL_ACTIVE_UNIFORM_MAX_LENGTH);

    int len;
    int size;
    GLenum type;

    for (int i = 0; i < n_uniforms; ++i) {
        string buf(uni_maxlen, '\0');
        glGetActiveUniform(id(), i, uni_maxlen, &len, &size, &type, buf.data());
        buf.resize(len);
        auto loc = glGetUniformLocation(id(), buf.c_str());

        // if loc == -1 its a block uniform.
        if (loc != -1) {
            LOG_TRACE_L1("uniform: {} {}", gl_str(type), buf);
        }
        _uniforms.emplace(std::piecewise_construct, tuple{std::move(buf)},
                          tuple{loc, type, size});
    }

    int n_block_uniforms;
    vector<pair<string, block_info>> binfos;

    for (int i = 0; i < n_blocks; ++i) {
        string buf(blk_maxlen, '\0');
        glGetActiveUniformBlockName(id(), i, blk_maxlen, &len, buf.data());
        buf.resize(len);
        _blocks.emplace(buf);
        LOG_TRACE_L1("block: {}", buf);
        glGetActiveUniformBlockiv(id(), i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
        glGetActiveUniformBlockiv(id(), i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
                                  &n_block_uniforms);
        auto& slot =
            binfos.emplace_back(std::piecewise_construct, tuple{std::move(buf)},
                                tuple{size, vector<GLenum>(n_block_uniforms)});
        auto& indices = slot.second.types;
        glGetActiveUniformBlockiv(id(), i,
                                  GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
                                  reinterpret_cast<int*>(indices.data()));
        for (int j = 0; j < n_block_uniforms; ++j) {
            glGetActiveUniform(id(), indices[j], 0, nullptr, nullptr,
                               &indices[j], nullptr);
        }
    }
    return binfos;
}

// =====[Pipeline]=====

const Pipeline Pipeline::null{nullptr};

void Pipeline::attach(const Shader& sh)
{
    using hera::swap;
    using enum shader_t::value_type;
    if (sh.type() == vertex) {
        if (_vert.id() != 0) {
            LOG_INFO("replacing {} with {} in {}", _vert.filename(),
                     sh.filename(), name());
        }
        _vert = sh;
    }
    else if (sh.type() == fragment) {
        if (_frag.id() != 0) {
            LOG_INFO("replacing {} with {} in {}", _frag.filename(),
                     sh.filename(), name());
        }
        _frag = sh;
    }
    else {
        throw hera::runtime_error("unexpected shader type");
    }
    gl::attach(id(), sh.id(), sh.type());
}

string Pipeline::info_log() const
{
    auto len = gl::parameter(id(), GL_INFO_LOG_LENGTH);
    string info(len - 1, '\0');
    glGetProgramPipelineInfoLog(id(), len, nullptr, info.data());
    return info;
}

bool Pipeline::validate() const
{
    glValidateProgramPipeline(id());
    return gl::parameter(id(), GL_VALIDATE_STATUS);
}

// =====[Shaders]=====

void Shaders::define(const string& key, const string& value)
{
    _preprocessor.define(key, value);
}

const Shader& Shaders::loadf(const path& pat)
{
    // if it already exists, call load again.
    auto&& sh = _shaders.emplace(pat.native(), pat).first->second;
    sh.load(*this);
    auto blk_info = sh.build_cache();
    for (auto&& [bname, binfo] : blk_info) {
        auto [it, newblk] = _block_infos.emplace(bname, binfo);
        if (!newblk) {
            auto& expect = it->second;
            if (binfo.size != expect.size) {
                LOG_ERROR(
                    "uniform block size mismatch: {} (expect: {}, got: {})",
                    bname, expect.size, binfo.size);
            }
            if (binfo.types != expect.types) {
                auto got_tys = binfo.types | views::transform([](auto&& i) {
                                   return gl_str(i);
                               });
                auto exp_tys = expect.types | views::transform([](auto&& i) {
                                   return gl_str(i);
                               });
                auto msg =
                    fmt::format("(expect: {}, got: {})", exp_tys, got_tys);
                LOG_ERROR("uniform block type mismatch: {} {}", bname, msg);
            }
        }
    }
    return sh;
}

void Shaders::load(const path& pat)
{
    if (!fs::is_directory(pat)) {
        LOG_CRITICAL("Shaders::load invalid directory: {}", pat);
        throw hera::runtime_error("Shaders::load invalid directory");
    }
    // modname -> shaders
    hash_map<string_view, vector<const Shader*>> touched;

    for (const auto& dirent :
         fs::directory_iterator{pat} | views::filter([](auto&& de) {
             return de.is_regular_file() && Shader::classify(de.path()).valid();
         })) {
        auto& sh = loadf(dirent.path());
        bind_blocks_to(sh);
        auto& it = touched.insert({sh.modname(), {}}).first->second;
        it.push_back(&sh);
    }
    for (const auto& [modname, shaders] : touched) {
        auto n = shaders.size();
        if (n == 1) {
            LOG_WARNING("lonely shader: {}", shaders.front()->filename());
            continue;
        }
        else if (n > 2) {
            LOG_WARNING("excess shaders: {}", modname);
        }
        auto& pipe = _pipelines.emplace(modname, modname).first->second;
        for (auto sh : shaders) {
            pipe.attach(*sh);
        }
    }
}

void Shaders::load()
{
    for (auto& sh : views::values(_shaders)) {
        sh.load(*this);
    }
}

const Pipeline& Shaders::active() const
{
    return *_active;
}

const Pipeline& Shaders::active(string_view name)
{
    auto elt = _pipelines.find(name);
    if (elt == _pipelines.end()) {
        LOG_CRITICAL("active() with non-existent program: {}", name);
        throw gl_error("non-existent program");
    }
    auto&& program = elt->second;
    program.bind();
    _active = &program;
    return program;
}

void Shaders::bind_block(const UniformBuffer<>& buf) const
{
    for (auto&& [pat, sh] : _shaders) {
        if (sh._blocks.contains(buf.name())) {
            buf.bind_to(sh);
        }
    }
}

void Shaders::bind_blocks_to(const Shader& prog) const
{
    for (auto&& bname : prog._blocks) {
        if (auto it = _blocks.find(bname); it != _blocks.end()) {
            it->second.bind_to(prog);
        }
    }
}

} // namespace hera::gl
