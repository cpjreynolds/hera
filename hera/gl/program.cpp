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

#include <hera/log.hpp>
#include <hera/gl/program.hpp>

namespace hera::gl {

namespace {
// ingests a shader into a "friendly" format.
// returns the source string and a map from file numbers to names.
pair<string, map<string, string>> ingest_shader(istream& src)
{
    string retsrc;
    // need to map in both directions.
    map<string, string> fn_num;
    map<string, string> num_fn;

    for (string line; std::getline(src, line);) {
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
                num_fn.insert({fnum, fname});
            }
            else {
                // not a new file. use its existing number.
                fnum = elt->second;
            }
            line.replace(qi, ei, fnum);
        }
        retsrc.append(line);
        retsrc.push_back('\n');
    }
    return {retsrc, num_fn};
}

string fixup_infolog(const string& ilog, const map<string, string>& fnmap)
{
    istringstream iss{ilog};
    string output;
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
        auto fnum = line.substr(idx1, idx2 - idx1);
        line.replace(idx1, idx2 - idx1, fnmap.at(fnum));
        output.append(line);
        output.push_back('\n');
    }
    // remove the last newline.
    output.pop_back();
    return output;
}

shader_t shader_classify(const path& fpath)
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

} // namespace

// =====[Shader]=====

const Shader Shader::null{nullptr};

Shader::Shader(path pat)
    : _fpath{std::move(pat)},
      _fname{_fpath.filename()},
      _type{shader_classify(_fpath)}
{
    if (+_type == GL_INVALID_ENUM) {
        LOG_CRITICAL("indeterminate shader type: {}", _fpath.filename());
        throw hera::runtime_error("indeterminate shader type");
    }
}

string_view Shader::modname() const
{
    string_view name = _fname;
    auto loc = name.find('.');
    name.remove_suffix(name.size() - loc);
    return name;
}

string Shader::info_log() const
{
    auto len = query(GL_INFO_LOG_LENGTH);
    string info(len, '\0');
    glGetProgramInfoLog(id(), len, nullptr, info.data());
    return info;
}

void Shader::load() const
{
    if (!_uniforms.empty()) {
        LOG_INFO("reloading shader: {}", _fpath.filename());
        _uniforms.clear();
        _blocks.clear();
    }
    else {
        LOG_INFO("loading shader: {}", _fpath.filename());
    }

    ifstream ifile{_fpath};
    ifile.exceptions(ifstream::badbit);
    auto [src, fnmap] = ingest_shader(ifile);

    auto sID = glCreateShader(+_type);

    auto data = src.data();
    glShaderSource(sID, 1, &data, nullptr);
    glCompileShader(sID);

    int param;
    glGetShaderiv(sID, GL_COMPILE_STATUS, &param);
    if (!param) {
        glGetShaderiv(sID, GL_INFO_LOG_LENGTH, &param);
        string info(param, '\0');
        glGetShaderInfoLog(sID, param, nullptr, info.data());
        auto fixed_info = fixup_infolog(info, fnmap);
        LOG_ERROR("\n{}", fixed_info);
        glDeleteShader(sID);
        throw gl_error("shader compile error");
    }

    gl::parameter(id(), GL_PROGRAM_SEPARABLE, true);
    glAttachShader(id(), sID);
    glLinkProgram(id());
    glDetachShader(id(), sID);
    glDeleteShader(sID);

    // error checking
    if (!query(GL_LINK_STATUS)) {
        LOG_ERROR("shader link error\n{}", info_log());
        throw hera::runtime_error("shader link error");
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
    string info(len, '\0');
    glGetProgramPipelineInfoLog(id(), len, nullptr, info.data());
    return info;
}

bool Pipeline::valid() const
{
    glValidateProgramPipeline(id());
    return gl::parameter(id(), GL_VALIDATE_STATUS);
}

// =====[Programs]=====

const Shader& Shaders::loadf(const path& pat)
{
    // if it already exists, call load again.
    auto&& sh = _shaders.emplace(pat.native(), pat).first->second;
    sh.load();
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
             return de.is_regular_file() && shader_classify(de.path()).valid();
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
