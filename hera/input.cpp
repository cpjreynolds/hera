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

#include <hera/window.hpp>
#include <hera/input.hpp>
#include <hera/init.hpp>
#include <hera/config.hpp>
#include <hera/ui.hpp>
#include <hera/loader.hpp>

using scancode_t = hera::key_event::scancode_t;
using modifier_t = hera::key_event::modifier_t;
using keycode_t = hera::key_event::keycode_t;

namespace hera {

namespace {
// scancode -> key LUT
static keycode_t scancode_map[GLFW_KEY_LAST];

static unordered_map<string, key_event> string_keys;

void init_string_key_map()
{
    for (uint16_t scan = 0u; scan <= key_event::scan_max; ++scan) {
        for (uint16_t mod = 0u; mod <= key_event::mod_all; ++mod) {
            key_event kup(scancode_t{scan}, modifier_t{mod}, false);
            key_event kdn(scancode_t{scan}, modifier_t{mod}, true);
            string_keys.emplace(kup.str(), kup);
            string_keys.emplace(kdn.str(), kdn);
        }
    }
}

void init_scancode_map()
{
    for (uint16_t i = 0; i < (uint16_t)sizeof(scancode_map); ++i) {
        auto sc = glfwGetKeyScancode(i);
        if (sc != -1) {
            assert(sc < GLFW_KEY_LAST);
            scancode_map[sc] = keycode_t{i};
        }
    }
}
} // namespace

void init::input()
{
    LOG_DEBUG("init input");
    init_scancode_map();
    init_string_key_map();
    input_atlas::init();
    input::init();
    LOG_DEBUG("init input done");
}

void deinit::input()
{
    input_atlas::deinit();
}

string key_event::key_str() const
{
    if (is_mouse()) {
        switch (scan) {
        case mouse1:
            return "mouse1";
        case mouse2:
            return "mouse2";
        case mouse3:
            return "mouse3";
        case mouse4:
            return "mouse4";
        case mouse5:
            return "mouse5";
        case mouse6:
            return "mouse6";
        case mouse7:
            return "mouse7";
        case mouse8:
            return "mouse8";
        default:
            std::unreachable();
        }
    }
    const char* name = glfwGetKeyName(GLFW_KEY_UNKNOWN, scan);
    if (name && std::isgraph(*name)) {
        return name;
    }
    // non printables
    switch (scancode_map[scan]) {
    case key_escape:
        return "esc";
    case key_space:
        return "space";
    case key_enter:
        return "enter";
    case key_tab:
        return "tab";
    case key_left_shift:
        return "lshift";
    case key_right_shift:
        return "rshift";
    case key_left_alt:
        return "lalt";
    case key_right_alt:
        return "ralt";
    case key_left_super:
        return "lsuper";
    case key_right_super:
        return "rsuper";
    case key_left_control:
        return "lctrl";
    case key_right_control:
        return "rctrl";
    case key_backspace:
        return "backspace";
    case key_f1:
        return "f1";
    case key_f2:
        return "f2";
    case key_f3:
        return "f3";
    case key_f4:
        return "f4";
    case key_f5:
        return "f5";
    case key_f6:
        return "f6";
    case key_f7:
        return "f7";
    case key_f8:
        return "f8";
    case key_f9:
        return "f9";
    case key_f10:
        return "f10";
    case key_f11:
        return "f11";
    case key_f12:
        return "f12";
    case key_f13:
        return "f13";
    case key_f14:
        return "f14";
    case key_f15:
        return "f15";
    case key_f16:
        return "f16";
    case key_f17:
        return "f17";
    case key_f18:
        return "f18";
    case key_f19:
        return "f19";
    case key_f20:
        return "f20";
    case key_f21:
        return "f21";
    case key_f22:
        return "f22";
    case key_f23:
        return "f23";
    case key_f24:
        return "f24";
    case key_f25:
        return "f25";
    case key_world_1:
        return "world1";
    case key_world_2:
        return "world2";
    default:
        return "unknown";
    }
}

string key_event::mod_str() const
{
    if (mod == mod_any) {
        return "any+";
    }
    string buf;
    if (mod & mod_ctrl) {
        buf.append("ctrl+");
    }
    if (mod & mod_alt) {
        buf.append("alt+");
    }
    if (mod & mod_shift) {
        buf.append("shift+");
    }
    if (mod & mod_super) {
        buf.append("super+");
    }
    if (mod & mod_capslk) {
        buf.append("caps+");
    }
    if (mod & mod_numlk) {
        buf.append("numlk+");
    }
    return buf;
}

string key_event::str() const
{
    if (invalid()) {
        return "invalid";
    }
    string buf = mod_str();
    buf += key_str();
    buf += '.';
    buf += state_str();
    return buf;
}

// (up, down) pair. returns invalid key_event if mapping fails.
pair<key_event, key_event> key_event::from_toml(const toml::table& table)
{
    optional<key_event> keyup = nullopt;
    optional<key_event> keydn = nullopt;

    if (auto kup_n = table["up"], kdn_n = table["dn"]; kup_n && kdn_n) {
        auto map_s = [](auto&& s) -> optional<key_event> {
            if (auto&& k = string_keys.find(s); k == string_keys.end()) {
                return k->second;
            }
            else {
                return nullopt;
            }
        };

        // first check for "up" and "dn" key strings.
        keyup = kup_n.value_exact<string>().and_then(map_s);
        keydn = kdn_n.value_exact<string>().and_then(map_s);
    }

    if (auto maybe_k = table["key"].value_exact<string>(); maybe_k) {
        auto map_kstr = [](auto&& kstr) {
            auto it = string_keys.find(kstr), end = string_keys.end();
            return [=] { return it != end ? optional{it->second} : nullopt; };
        };

        // "key" string covers up and down but doesn't override "up" or "dn".
        keyup = keyup.or_else(map_kstr(*maybe_k + ".up"));
        keydn = keydn.or_else(map_kstr(*maybe_k + ".dn"));
    }

    auto scanup_n = table.at_path("scancode.up");
    auto scandn_n = table.at_path("scancode.dn");

    if (scanup_n && scandn_n) {
        auto map_i = [](auto&& i) { return key_event(i); };
        auto scanup = scanup_n.value_exact<int64_t>().transform(map_i);
        auto scandn = scandn_n.value_exact<int64_t>().transform(map_i);

        // if the scancode is present, and different, override.
        keyup = keyup == scanup ? keyup : scanup;
        keydn = keydn == scandn ? keydn : scandn;
    }

    return {keyup.value_or(invalid_key), keydn.value_or(invalid_key)};
}

toml::table& key_event::insert_into(toml::table& table) const
{

    auto&& scantable =
        table.emplace("scancode", toml::table{}).first->second.as_table();
    scantable->insert(state_str(), int64_t{data});
    table.insert(state_str(), str());
    return table;
}

GLFWwindow* input::window = nullptr;
GLFWmonitor* input::monitor = nullptr;

signal<void(key_event)> input::keys{};
signal<void(input_action)> input::actions{};
signal<void(vec2, vec2)> input::cursor{};
signal<void(vec2)> input::scroll{};
signal<void(ivec2)> input::fbsize{};
signal<void(ivec2)> input::winsize{};
signal<void(vec2)> input::cscale{};

vec2 input::_cursor_pos = {0, 0};

bool input::_capturing = false;
input::cmode input::_cursor_mode = cursor_normal;

ivec2 input::_winsize = {0, 0};
ivec2 input::_fbsize = {0, 0};
vec2 input::_cscale = {0, 0};

uvec2 input::_dpi = {0, 0};
ivec2 input::_monitor_mm = {0, 0};

mat3 input::_virt2ndc{};
mat3 input::_pix2ndc{};
mat3 input::_nss2ndc{};

mat3 input::_ndc2virt{};
mat3 input::_ndc2pix{};
mat3 input::_ndc2nss{};

uint8_t input::modstate[key_event::scan_max + 1];

void input::init()
{
    window = glfwGetCurrentContext();
    monitor = glfwGetPrimaryMonitor();

    glfwGetMonitorPhysicalSize(monitor, &_monitor_mm.x, &_monitor_mm.y);

    glfwGetWindowSize(window, &_winsize.x, &_winsize.y);
    glfwGetFramebufferSize(window, &_fbsize.x, &_fbsize.y);
    glfwGetWindowContentScale(window, &_cscale.x, &_cscale.y);

    make_maps();

    window_scale_cb(window, _cscale.x, _cscale.y);

    glfwSetWindowFocusCallback(window, &window_focus_cb);
    glfwSetMouseButtonCallback(window, &mouse_btn_cb);
    glfwSetCursorPosCallback(window, &cursor_pos_cb);
    glfwSetCursorEnterCallback(window, &cursor_enter_cb);
    glfwSetScrollCallback(window, &scroll_cb);
    glfwSetKeyCallback(window, &key_input_cb);
    glfwSetWindowSizeCallback(window, &window_size_cb);
    glfwSetFramebufferSizeCallback(window, &fb_size_cb);
    glfwSetWindowContentScaleCallback(window, &window_scale_cb);

    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, true);
    }

    capture_cursor();
}

void input::flush()
{
    keys.flush();
    actions.flush();
    cursor.flush();
    scroll.flush();
    fbsize.flush();
    winsize.flush();
    cscale.flush();
}

vec2 input::cursor_pos()
{
    return _cursor_pos;
}

vec2 input::get_cursor()
{
    double cx, cy;
    glfwGetCursorPos(window, &cx, &cy);
    return virtual2nss({cx, cy});
}

void input::set_cursor(vec2 pos)
{
    vec2 virt = nss2virtual(pos);
    glfwSetCursorPos(window, virt.x, virt.y);
}

ivec2 input::framebuffer_size()
{
    return _fbsize;
}

ivec2 input::window_size()
{
    return _winsize;
}

vec2 input::content_scale()
{
    return _cscale;
}

uvec2 input::dpi()
{
    return _dpi;
}

int input::refresh_rate()
{
    auto mode = glfwGetVideoMode(monitor);
    return mode->refreshRate;
}

bool input::should_close()
{
    return glfwWindowShouldClose(window);
}

void input::should_close(bool v)
{
    glfwSetWindowShouldClose(window, v);
}

bool input::has_focus()
{
    return glfwGetWindowAttrib(window, GLFW_FOCUSED);
}

bool input::has_hover()
{
    return glfwGetWindowAttrib(window, GLFW_HOVERED);
}

void input::toggle_cursor()
{
    if (_cursor_mode == _capture_mode) {
        _cursor_mode = cursor_normal;
    }
    else {
        _cursor_mode = _capture_mode;
    }
    if (_capturing) {
        glfwSetInputMode(window, GLFW_CURSOR, _cursor_mode);
        _cursor_pos = get_cursor();
    }
    auto& io = ImGui::GetIO();
    io.ConfigFlags ^= ImGuiConfigFlags_NoMouse;
    io.ConfigFlags ^= ImGuiConfigFlags_NoKeyboard;
}

void input::capture_cursor()
{
    glfwSetInputMode(window, GLFW_CURSOR, _cursor_mode);
    _cursor_pos = get_cursor();
    _capturing = true;
}

void input::release_cursor()
{
    glfwSetInputMode(window, GLFW_CURSOR, cursor_normal);
    _capturing = false;
}

void input::window_focus_cb(GLFWwindow*, int focused)
{
    if (focused) {
        LOG_DEBUG("window gained focus");
        set_cursor(vec2{0.5});
        capture_cursor();
    }
    else {
        LOG_DEBUG("window lost focus");
        release_cursor();
    }
}

void input::cursor_enter_cb(GLFWwindow*, int entered)
{
    if (entered) {
        LOG_DEBUG("have cursor");
        capture_cursor();
    }
    else {
        LOG_DEBUG("lost cursor");
        release_cursor();
    }
}

void input::cursor_pos_cb(GLFWwindow*, double xpos, double ypos)
{
    if (!_capturing)
        return;

    vec2 pos = virtual2nss({xpos, ypos});

    if (!ImGui::GetIO().WantCaptureMouse) {
        cursor.post(_cursor_pos - pos, pos);
    }
    _cursor_pos = pos;
}

void input::scroll_cb(GLFWwindow*, double xoffset, double yoffset)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    scroll.post(vec2{xoffset, yoffset});
}

void input::key_input_cb(GLFWwindow*, int key, int _scancode, int action,
                         int mods)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
        return;

    using enum keycode_t;
    using enum modifier_t;
    auto scancode = static_cast<scancode_t>(_scancode);
    if (action == GLFW_REPEAT) {
        return;
    }
    else if (action == GLFW_PRESS) {
        switch (key) {
        case key_left_shift:
        case key_right_shift:
            mods &= ~mod_shift;
            break;
        case key_left_alt:
        case key_right_alt:
            mods &= ~mod_alt;
            break;
        case key_left_super:
        case key_right_super:
            mods &= ~mod_super;
            break;
        case key_left_control:
        case key_right_control:
            mods &= ~mod_ctrl;
            break;
        default:
            break;
        }
        modstate[scancode] = mods;
    }
    else { // GLFW_RELEASE
        mods = modstate[scancode];
    }

    auto kev = key_event(scancode, static_cast<modifier_t>(mods), action);
    keys.post(kev);

    auto& atlas = input_atlas::global();
    if (auto action = atlas[kev]) {
        actions.post(*action);
    }
}

void input::mouse_btn_cb(GLFWwindow*, int btn, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    if (action == GLFW_REPEAT) {
        return;
    }
    auto scan = static_cast<scancode_t>(scancode_t::scan_max - btn);

    if (action == GLFW_PRESS) {
        modstate[scan] = mods;
    }
    else { // GLFW_RELEASE
        mods = modstate[scan];
    }

    auto kev = key_event(scan, static_cast<modifier_t>(mods), action);
    keys.post(kev);
    auto& atlas = input_atlas::global();
    if (auto action = atlas[kev]) {
        actions.post(*action);
    }
}

void input::window_size_cb(GLFWwindow*, int w, int h)
{
    _winsize = {w, h};
    make_virtual_maps();
    winsize.post(_winsize);
}

void input::fb_size_cb(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
    _fbsize = {w, h};
    make_pixel_maps();
    fbsize.post(_fbsize);
}

void input::window_scale_cb(GLFWwindow*, float x, float y)
{
    _cscale = {x, y};
    auto mode = glfwGetVideoMode(monitor);
    ivec2 res = {mode->width, mode->height};
    ivec2 dpmm = res / _monitor_mm;
    _dpi = (127 * dpmm) / 5;
    _dpi *= _cscale;

    cscale.post(_cscale);
}

// input map

input_map::map_t input_map::from_toml(const toml::table& table)
{
    map_t mapping;

    table.for_each([&](const toml::key& k, const toml::table& t) {
        auto&& [up, dn] = key_event::from_toml(t);
        if (up.invalid() || dn.invalid()) {
            LOG_ERROR("failed to map input: {}", table);
            throw hera::runtime_error("failed to map input");
        }
        mapping.emplace(up, input_action{k, false});
        mapping.emplace(dn, input_action{k, true});
    });
    return mapping;
}

input_map::map_t input_map::make_default()
{
    map_t mapping{std::from_range, input_action::defaults()};
    return mapping;
}

input_map::operator toml::table() const
{
    toml::table table;

    for (auto&& [k, v] : mapping) {
        auto&& act_tab =
            table.emplace(v.str(), toml::table{}).first->second.as_table();
        k.insert_into(*act_tab);
    }
    table.for_each([](toml::table& t) {
        auto s1 = *t.at("up").as_string();
        auto s2 = *t.at("dn").as_string();
        string_view upstr{s1->data(), s1->size()};
        string_view dnstr{s2->data(), s2->size()};
        upstr.remove_suffix(3);
        dnstr.remove_suffix(3);
        if (upstr == dnstr) {
            string samestr(upstr);
            t.erase("up");
            t.erase("dn");
            t.emplace("key", samestr);
        }
    });
    return table;
}

// input atlas

input_atlas* input_atlas::global_atlas = nullptr;

void input_atlas::init()
{
    LOG_DEBUG("init input_atlas");
    auto keymap_path = path_resolver::get().apply("config:/keymap.toml");
    if (keymap_path.empty()) {
        throw hera::runtime_error("no keymap path");
    }
    if (global_atlas != nullptr) {
        delete global_atlas;
    }
    global_atlas = new input_atlas{keymap_path};
    LOG_DEBUG("init input_atlas done");
}

void input_atlas::deinit()
{
    LOG_DEBUG("deinit input_atlas");
    delete global_atlas;
    LOG_DEBUG("deinit input_atlas done");
}

input_atlas& input_atlas::global()
{
    return *global_atlas;
}

input_atlas::input_atlas(const toml::table& table)
    : _fpath{*table.source().path}
{
    load(table);
}

input_atlas::input_atlas(const path& fpath) : _fpath{fpath}
{
    if (!fs::exists(fpath)) {
        atlas.emplace("default", input_map{});
        save(fpath);
    }
    else {
        load(toml::parse_file(_fpath.native()));
    }
    push("default", false);
}

void input_atlas::load(const toml::table& table)
{
    table.for_each([&](const toml::key& k, const toml::table& t) {
        atlas.emplace(k, input_map{t});
    });
}

void input_atlas::save(const path& fpath) const
{
    LOG_INFO("saving keymap to: {}", fpath);
    ofstream ofile{fpath, ios_base::trunc};
    ofile << toml::table(*this);
}

input_atlas::operator toml::table() const
{
    toml::table table;
    for (auto&& [k, v] : atlas) {
        table.emplace(k, toml::table(v));
    }
    return table;
}

void input_atlas::push(string_view mapname, bool transparent)
{
    auto&& i = atlas.find(mapname);
    if (i == atlas.end()) {
        throw hera::out_of_range("input map not found");
    }
    map_stack.emplace_back(&i->second, transparent);
}

void input_atlas::pop()
{
    if (depth() > 1) {
        map_stack.pop_back();
    }
}

size_t input_atlas::depth() const
{
    return map_stack.size();
}

} // namespace hera
