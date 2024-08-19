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

#ifndef HERA_INPUT_HPP
#define HERA_INPUT_HPP

#include <GLFW/glfw3.h>

#include <hera/common.hpp>
#include <hera/error.hpp>
#include <hera/toml.hpp>
#include <hera/log.hpp>
#include <hera/lua.hpp>
#include <hera/format.hpp>
#include <hera/config.hpp>
#include <hera/event.hpp>

namespace hera {

// A keypress event
struct key_event {
    enum scancode_t : uint16_t {
        scan_invalid = 0x1ff,
        scan_max = scan_invalid - 1,
        mouse1 = scan_max - GLFW_MOUSE_BUTTON_1,
        mouse2 = scan_max - GLFW_MOUSE_BUTTON_2,
        mouse3 = scan_max - GLFW_MOUSE_BUTTON_3,
        mouse4 = scan_max - GLFW_MOUSE_BUTTON_4,
        mouse5 = scan_max - GLFW_MOUSE_BUTTON_5,
        mouse6 = scan_max - GLFW_MOUSE_BUTTON_6,
        mouse7 = scan_max - GLFW_MOUSE_BUTTON_7,
        mouse8 = scan_max - GLFW_MOUSE_BUTTON_8,
    };
    enum modifier_t : uint16_t {
        mod_none = 0,
        mod_alt = GLFW_MOD_ALT,
        mod_shift = GLFW_MOD_SHIFT,
        mod_super = GLFW_MOD_SUPER,
        mod_ctrl = GLFW_MOD_CONTROL,
        mod_capslk = GLFW_MOD_CAPS_LOCK,
        mod_numlk = GLFW_MOD_NUM_LOCK,
        mod_all =
            mod_alt | mod_shift | mod_super | mod_ctrl | mod_capslk | mod_numlk,
        mod_any = mod_all,
    };
    enum keycode_t : uint16_t {
        key_space = GLFW_KEY_SPACE,
        key_apostrophe = GLFW_KEY_APOSTROPHE,
        key_comma = GLFW_KEY_COMMA,
        key_minus = GLFW_KEY_MINUS,
        key_period = GLFW_KEY_PERIOD,
        key_slash = GLFW_KEY_SLASH,
        key_0 = GLFW_KEY_0,
        key_1 = GLFW_KEY_1,
        key_2 = GLFW_KEY_2,
        key_3 = GLFW_KEY_3,
        key_4 = GLFW_KEY_4,
        key_5 = GLFW_KEY_5,
        key_6 = GLFW_KEY_6,
        key_7 = GLFW_KEY_7,
        key_8 = GLFW_KEY_8,
        key_9 = GLFW_KEY_9,
        key_semicolon = GLFW_KEY_SEMICOLON,
        key_equals = GLFW_KEY_EQUAL,
        key_a = GLFW_KEY_A,
        key_b = GLFW_KEY_B,
        key_c = GLFW_KEY_C,
        key_d = GLFW_KEY_D,
        key_e = GLFW_KEY_E,
        key_f = GLFW_KEY_F,
        key_g = GLFW_KEY_G,
        key_h = GLFW_KEY_H,
        key_i = GLFW_KEY_I,
        key_j = GLFW_KEY_J,
        key_k = GLFW_KEY_K,
        key_l = GLFW_KEY_L,
        key_m = GLFW_KEY_M,
        key_n = GLFW_KEY_N,
        key_o = GLFW_KEY_O,
        key_p = GLFW_KEY_P,
        key_q = GLFW_KEY_Q,
        key_r = GLFW_KEY_R,
        key_s = GLFW_KEY_S,
        key_t = GLFW_KEY_T,
        key_u = GLFW_KEY_U,
        key_v = GLFW_KEY_V,
        key_w = GLFW_KEY_W,
        key_x = GLFW_KEY_X,
        key_y = GLFW_KEY_Y,
        key_z = GLFW_KEY_Z,
        key_left_bracket = GLFW_KEY_LEFT_BRACKET,
        key_backslash = GLFW_KEY_BACKSLASH,
        key_right_bracket = GLFW_KEY_RIGHT_BRACKET,
        key_grave_accent = GLFW_KEY_GRAVE_ACCENT,
        key_world_1 = GLFW_KEY_WORLD_1,
        key_world_2 = GLFW_KEY_WORLD_2,
        // noprint,
        key_escape = GLFW_KEY_ESCAPE,
        key_enter = GLFW_KEY_ENTER,
        key_tab = GLFW_KEY_TAB,
        key_backspace = GLFW_KEY_BACKSPACE,
        key_insert = GLFW_KEY_INSERT,
        key_delete = GLFW_KEY_DELETE,
        key_right = GLFW_KEY_RIGHT,
        key_left = GLFW_KEY_LEFT,
        key_down = GLFW_KEY_DOWN,
        key_up = GLFW_KEY_UP,
        key_page_up = GLFW_KEY_PAGE_UP,
        key_page_down = GLFW_KEY_PAGE_DOWN,
        key_home = GLFW_KEY_HOME,
        key_end = GLFW_KEY_END,
        key_caps_lock = GLFW_KEY_CAPS_LOCK,
        key_scroll_lock = GLFW_KEY_SCROLL_LOCK,
        key_num_lock = GLFW_KEY_NUM_LOCK,
        key_print_screen = GLFW_KEY_PRINT_SCREEN,
        key_pause = GLFW_KEY_PAUSE,
        key_f1 = GLFW_KEY_F1,
        key_f2 = GLFW_KEY_F2,
        key_f3 = GLFW_KEY_F3,
        key_f4 = GLFW_KEY_F4,
        key_f5 = GLFW_KEY_F5,
        key_f6 = GLFW_KEY_F6,
        key_f7 = GLFW_KEY_F7,
        key_f8 = GLFW_KEY_F8,
        key_f9 = GLFW_KEY_F9,
        key_f10 = GLFW_KEY_F10,
        key_f11 = GLFW_KEY_F11,
        key_f12 = GLFW_KEY_F12,
        key_f13 = GLFW_KEY_F13,
        key_f14 = GLFW_KEY_F14,
        key_f15 = GLFW_KEY_F15,
        key_f16 = GLFW_KEY_F16,
        key_f17 = GLFW_KEY_F17,
        key_f18 = GLFW_KEY_F18,
        key_f19 = GLFW_KEY_F19,
        key_f20 = GLFW_KEY_F20,
        key_f21 = GLFW_KEY_F21,
        key_f22 = GLFW_KEY_F22,
        key_f23 = GLFW_KEY_F23,
        key_f24 = GLFW_KEY_F24,
        key_f25 = GLFW_KEY_F25,
        key_kp_0 = GLFW_KEY_KP_0,
        key_kp_1 = GLFW_KEY_KP_1,
        key_kp_2 = GLFW_KEY_KP_2,
        key_kp_3 = GLFW_KEY_KP_3,
        key_kp_4 = GLFW_KEY_KP_4,
        key_kp_5 = GLFW_KEY_KP_5,
        key_kp_6 = GLFW_KEY_KP_6,
        key_kp_7 = GLFW_KEY_KP_7,
        key_kp_8 = GLFW_KEY_KP_8,
        key_kp_9 = GLFW_KEY_KP_9,
        key_kp_decimal = GLFW_KEY_KP_DECIMAL,
        key_kp_divide = GLFW_KEY_KP_DIVIDE,
        key_kp_multiply = GLFW_KEY_KP_MULTIPLY,
        key_kp_subtract = GLFW_KEY_KP_SUBTRACT,
        key_kp_add = GLFW_KEY_KP_ADD,
        key_kp_enter = GLFW_KEY_KP_ENTER,
        key_kp_equal = GLFW_KEY_KP_EQUAL,
        key_left_shift = GLFW_KEY_LEFT_SHIFT,
        key_left_control = GLFW_KEY_LEFT_CONTROL,
        key_left_alt = GLFW_KEY_LEFT_ALT,
        key_left_super = GLFW_KEY_LEFT_SUPER,
        key_right_shift = GLFW_KEY_RIGHT_SHIFT,
        key_right_control = GLFW_KEY_RIGHT_CONTROL,
        key_right_alt = GLFW_KEY_RIGHT_ALT,
        key_right_super = GLFW_KEY_RIGHT_SUPER,
        key_menu = GLFW_KEY_MENU,
        key_last = GLFW_KEY_LAST,
    };

    static const key_event invalid_key;

    union {
        uint16_t data;
        struct {
            scancode_t scan : 9;
            modifier_t mod : 6;
            uint16_t press : 1;
        };
    };

    constexpr key_event() : data(-1) {};
    constexpr key_event(scancode_t scan, modifier_t mod, bool press)
        : scan{scan},
          mod{mod},
          press{press} {};
    constexpr key_event(keycode_t key, modifier_t mod, bool press)
        : scan{static_cast<scancode_t>(glfwGetKeyScancode(key))},
          mod{mod},
          press{press} {};

    constexpr key_event(scancode_t scan, bool press)
        : key_event{scan, mod_none, press} {};
    constexpr key_event(keycode_t key, bool press)
        : key_event{key, mod_none, press} {};

    explicit constexpr key_event(uint16_t v) : data{v} {};

    constexpr operator uint16_t() const { return data; }
    friend constexpr bool operator==(const key_event& l, const key_event& r)
    {
        return l.data == r.data;
        /*
        if (l.mod == any_mod || r.mod == any_mod) {
            return l.scan == r.scan && l.press == r.press;
        }
        else {
            return l.data == r.data;
        }
        */
    }

    constexpr bool valid() const { return *this != invalid_key; }
    constexpr bool invalid() const { return *this == invalid_key; }

    // (up, down) pair from a toml table
    static pair<key_event, key_event> from_toml(const toml::table&);

    toml::table& insert_into(toml::table&) const;

    // returns true if event is a mouse button
    bool is_mouse() const { return scan <= mouse1 && scan >= mouse8; }

    bool is_keyboard() const { return scan <= GLFW_KEY_LAST; }

    // returns a static string representing the key
    string key_str() const;
    // returns a string representing the modifiers
    string mod_str() const;
    // returns a static string representing the press state
    constexpr string_view state_str() const { return press ? "dn" : "up"; }
    // returns a string representing the key with modifiers and press state.
    string str() const;
    friend struct fmt::formatter<key_event>;
};
inline constexpr key_event key_event::invalid_key(-1);
static_assert(sizeof(key_event) == sizeof(uint16_t));
static_assert(GLFW_KEY_LAST < (1 << 9));
static_assert(GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT |
              GLFW_MOD_SUPER | GLFW_MOD_CAPS_LOCK | GLFW_MOD_NUM_LOCK == 0x3f);

#define HERA_INPUT_ACTIONS(X)                                                  \
    X(escape, key_escape)                                                      \
    X(move_fwd, key_w)                                                         \
    X(move_back, key_s)                                                        \
    X(move_left, key_a)                                                        \
    X(move_right, key_d)                                                       \
    X(move_up, key_space)                                                      \
    X(move_down, key_x)                                                        \
    X(click, mouse1)                                                           \
    X(roll_left, key_q)                                                        \
    X(roll_right, key_e)                                                       \
    X(reload, key_o)                                                           \
    X(toggle_mode, key_backspace)

#define MAKE_ACTIONS(VAR, DEFAULT) VAR,
#define MAKE_STRINGS(VAR, DEFAULT) #VAR,

#define MAKE_DEFAULTS(VAR, DEFAULT)                                            \
    pair{key_event{key_event::DEFAULT, true},                                  \
         input_action{input_action::VAR, true}},                               \
        pair{key_event{key_event::DEFAULT, false},                             \
             input_action{input_action::VAR, false}},

struct input_action {
    using underlying_t = uint8_t;
    enum value : underlying_t { HERA_INPUT_ACTIONS(MAKE_ACTIONS) };

    constexpr input_action() : _val{0x7f} {};
    constexpr input_action(value v, bool press) : _val{v}, _press{press} {};
    constexpr input_action(string_view v, bool press)
        : _val{from_str(v)},
          _press{press} {};

    constexpr operator value() const { return _val; }
    constexpr explicit operator bool() const { return valid(); }
    explicit operator toml::table() const;

    friend constexpr bool operator==(const input_action& l, const value& r)
    {
        return l._val == r;
    }
    friend constexpr bool operator==(const input_action& l,
                                     const input_action& r) = default;

    constexpr bool valid() const { return _val < value_max; }
    constexpr bool pressed() const { return _press; }
    constexpr bool down() const { return _press; }
    constexpr bool released() const { return !_press; }
    constexpr bool up() const { return !_press; }

private:
    value _val : sizeof(value) * 8 - 1;
    underlying_t _press : 1;

    static constexpr string_view value_str[] = {
        HERA_INPUT_ACTIONS(MAKE_STRINGS)};

public:
    static constexpr underlying_t value_max =
        sizeof(value_str) / sizeof(value_str[0]);
    static_assert(std::bit_width(value_max) < sizeof(value) * 8);

    static span<const pair<key_event, input_action>, value_max * 2> defaults()
    {
        static const pair<key_event, input_action> _default_actions[] = {
            HERA_INPUT_ACTIONS(MAKE_DEFAULTS)};
        return _default_actions;
    }

private:
    // (pressed, released) pairs of all actions excluding `none`
    static const array<input_action, value_max * 2> actions;

    static constexpr value from_str(string_view v)
    {
        for (underlying_t i = 0; i < value_max; ++i) {
            if (v == value_str[i])
                return value{i};
        }
        throw hera::out_of_range("bad input_action value str");
    }

    constexpr string_view str() const { return value_str[_val]; }
    constexpr string_view state_str() const { return _press ? "dn" : "up"; }
    friend struct input_map;
    friend struct fmt::formatter<input_action>;
};
static_assert(sizeof(input_action) == sizeof(input_action::underlying_t));

#define MAKE_ALL_ACTIONS(VAR, DEFAULT)                                         \
    input_action{input_action::VAR, true},                                     \
        input_action{input_action::VAR, false},

inline constexpr decltype(input_action::actions) input_action::actions = {
    HERA_INPUT_ACTIONS(MAKE_ALL_ACTIONS)};

#undef HERA_INPUT_ACTIONS
#undef MAKE_ACTIONS
#undef MAKE_STRINGS
#undef MAKE_ALL_ACTIONS
#undef MAKE_DEFAULTS

struct input_map {
    using hasher = trans_hash<uint16_t>;
    using map_t =
        unordered_map<key_event, input_action, hasher, std::equal_to<>>;

    input_map() : mapping{make_default()} {};
    input_map(const toml::table& t) : mapping{from_toml(t)} {};

    input_map(const input_map&) = delete;
    input_map& operator=(const input_map&) = delete;
    input_map(input_map&&) = default;
    input_map& operator=(input_map&&) = default;

    operator toml::table() const;

    const map_t& operator*() const { return mapping; }
    const map_t* operator->() const { return &mapping; }

private:
    map_t mapping;

    static map_t from_toml(const toml::table&);
    static map_t make_default();
    friend struct fmt::formatter<input_map>;
};

class input_atlas {
    using hasher = trans_hash<string_view>;
    using atlas_t = unordered_map<string, input_map, hasher, std::equal_to<>>;
    using stack_t = vector<pair<const input_map*, bool>>;
    friend struct fmt::formatter<input_atlas>;

    path _fpath;
    atlas_t atlas;
    stack_t map_stack;

    static input_atlas* global_atlas;

    // iterator over the chain of input maps, upto and including the first
    // non-transparent map.
    auto lookup_chain(bool& done) const
    {
        // done needs to be a reference argument because it must outlive the
        // lambda and the lambda may not be mutable in take_while so I can't
        // capture by value.
        return map_stack | views::reverse |
               views::take_while([&done](auto&& p) {
                   return !std::exchange(done, p.second);
               }) |
               views::keys |
               views::transform(
                   [](auto&& mp) -> const input_map& { return *mp; });
    }

public:
    static void init();
    static void deinit();
    static input_atlas& global();

    input_atlas(const toml::table&);
    input_atlas(const path&);
    ~input_atlas() { save(); }

    input_atlas(const input_atlas&) = delete;
    input_atlas& operator=(const input_atlas&) = delete;
    input_atlas(input_atlas&&) = default;
    input_atlas& operator=(input_atlas&&) = default;

    operator toml::table() const;

    // serialize the input atlas to disk.
    void save(const path&) const;
    void save() const { save(_fpath); }

    void load(const toml::table&);

    void push(string_view mapname, bool transparent = true);
    void pop();
    size_t depth() const;

    optional<input_action> operator[](key_event key) const
    {
        for (bool done = false; auto&& inmap : lookup_chain(done)) {
            if (auto i = inmap->find(key); i != inmap->end())
                return i->second;
        }
        return nullopt;
    }
};

struct input {
    static qsignal<void(key_event)> keys;
    static qsignal<void(input_action)> actions;
    // {cursor delta, cursor position}
    static qsignal<void(vec2, vec2)> cursor;
    static qsignal<void(vec2)> scroll;
    static qsignal<void(ivec2)> fbsize;
    static qsignal<void(ivec2)> winsize;
    static qsignal<void(vec2)> cscale;

    enum cmode {
        cursor_normal = GLFW_CURSOR_NORMAL,
        cursor_hidden = GLFW_CURSOR_HIDDEN,
        cursor_captured = GLFW_CURSOR_CAPTURED,
        cursor_disabled = GLFW_CURSOR_DISABLED,
    };

    input() = delete;
    static void init();

    // poll the system for events
    static void poll() { glfwPollEvents(); }

    // flush all input signals.
    static void flush();

    // returns the cursor (x, y).
    static vec2 cursor_pos();
    // gets the cursor position directly from the input system.
    static vec2 get_cursor();
    // sets the cursor position.
    static void set_cursor(vec2);

    // framebuffer size (w, h)
    static ivec2 framebuffer_size();

    // window size (w, h)
    static ivec2 window_size();

    // content scale (x, y)
    static vec2 content_scale();

    // NDC to pixel (framebuffer) coordinates.
    static constexpr vec2 ndc2pixel(const vec2& v)
    {
        vec2 rv = _ndc2pix * vec3{v, 1.0};
        return rv;
    }
    // NDC to virtual coordinates.
    static constexpr vec2 ndc2virtual(const vec2& v)
    {
        vec2 rv = _ndc2virt * vec3{v, 1.0};
        return rv;
    }
    // NDC to normalized screen space.
    static constexpr vec2 ndc2nss(const vec2& v)
    {
        vec2 rv = _ndc2nss * vec3{v, 1.0};
        return rv;
    }

    // pixel space to NDC.
    static constexpr vec2 pixel2ndc(const vec2& v)
    {
        vec2 rv = _pix2ndc * vec3{v, 1.0};
        return rv;
    }
    static constexpr vec2 pixel2virtual(const vec2& v)
    {
        vec2 rv = v / _cscale;
        return rv;
    }
    static constexpr vec2 pixel2nss(const vec2& v)
    {
        vec2 rv = v / vec2{_fbsize};
        return rv;
    }

    static constexpr vec2 virtual2ndc(const vec2& v)
    {
        vec2 rv = _virt2ndc * vec3{v, 1.0};
        return rv;
    }
    static constexpr vec2 virtual2pixel(const vec2& v)
    {
        vec2 rv = v * _cscale;
        return rv;
    }
    static constexpr vec2 virtual2nss(const vec2& v)
    {
        vec2 rv = v / vec2{_winsize};
        return rv;
    }

    static constexpr vec2 nss2ndc(const vec2& v)
    {
        vec2 rv = _nss2ndc * vec3{v, 1.0};
        return rv;
    }
    static constexpr vec2 nss2pixel(const vec2& v)
    {
        vec2 rv = v * vec2{_fbsize};
        return rv;
    }
    static constexpr vec2 nss2virtual(const vec2& v)
    {
        vec2 rv = v * vec2{_winsize};
        return rv;
    }

    // DPI based on primary monitor size and WINDOW content scale.
    static uvec2 dpi();

    static int refresh_rate();

    static bool should_close();
    static void should_close(bool);

    // true if the window has input focus.
    static bool has_focus();
    // true if the window has the cursor hovering over it.
    static bool has_hover();

    // cursor is captured.
    static void capture_cursor();
    // resets cursor mode.
    static void release_cursor();
    // toggle between capture mode and normal mode.
    static void toggle_cursor();

private:
    static GLFWwindow* window;
    static GLFWmonitor* monitor;

    // (x, y)
    static vec2 _cursor_pos;

    static constexpr cmode _capture_mode = cursor_disabled;
    static cmode _cursor_mode;

    static bool _capturing;

    // cached winsize (virtual)
    static ivec2 _winsize;
    // cached fbsize (pixels)
    static ivec2 _fbsize;
    // cached content scale
    static vec2 _cscale;

    // (primary) monitor physical dimensions
    static ivec2 _monitor_mm;

    static mat3 _virt2ndc;
    static mat3 _pix2ndc;
    static mat3 _nss2ndc;

    static mat3 _ndc2virt;
    static mat3 _ndc2pix;
    static mat3 _ndc2nss;

    static inline void make_maps()
    {
        make_pixel_maps();
        make_virtual_maps();
        _nss2ndc = make_ortho2d({1.f, 1.f});
        _ndc2nss = make_ortho2d_inv({1.f, 1.f});
    }

    static inline void make_pixel_maps()
    {
        _pix2ndc = make_ortho2d(_fbsize);
        _ndc2pix = make_ortho2d_inv(_fbsize);
    }

    static inline void make_virtual_maps()
    {
        _virt2ndc = make_ortho2d(_winsize);
        _ndc2virt = make_ortho2d_inv(_winsize);
    }

    static constexpr mat3 make_ortho2d(const vec2& dims)
    {
        mat3 m;
        m[0][0] = 2.f / dims.x;
        m[1][1] = 2.f / dims.y;
        m[2][0] = -1;
        m[2][1] = -1;
        return m;
    }

    static constexpr mat3 make_ortho2d_inv(const vec2& dims)
    {
        mat3 m;
        m[0][0] = dims.x / 2.f;
        m[1][1] = dims.y / 2.f;
        m[2][0] = dims.x / 2.f;
        m[2][1] = dims.y / 2.f;
        return m;
    }

    // DPI
    static uvec2 _dpi;

    // system callbacks.
    static void window_focus_cb(GLFWwindow*, int focused);
    static void window_size_cb(GLFWwindow*, int w, int h);
    static void mouse_btn_cb(GLFWwindow*, int btn, int action, int mods);
    static void cursor_pos_cb(GLFWwindow*, double xpos, double ypos);
    static void cursor_enter_cb(GLFWwindow*, int entered);
    static void scroll_cb(GLFWwindow*, double scrollx, double scrolly);
    static void key_input_cb(GLFWwindow*, int key, int scancode, int action,
                             int mods);
    static void fb_size_cb(GLFWwindow*, int w, int h);
    static void window_scale_cb(GLFWwindow*, float x, float y);

    // stores the modifier state when a key was last pressed
    static uint8_t modstate[key_event::scan_max + 1];
};

} // namespace hera

template<>
struct fmt::formatter<hera::key_event> : formatter<std::string_view> {
    auto format(const hera::key_event& val, auto& ctx) const
    {
        return formatter<std::string_view>::format(val.str(), ctx);
    }
};

template<>
struct fmt::formatter<hera::input_action> : formatter<std::string_view> {
    auto format(const hera::input_action& val, auto& ctx) const
    {
        return formatter<std::string_view>::format(
            fmt::format("{}.{}", val.value_str[val], val.state_str()), ctx);
    }
};

template<>
struct fmt::formatter<hera::input_map>
    : hera::format_parser<hera::input_map::map_t> {
    auto format(const hera::input_map& val, auto& ctx) const
    {
        if (alternate) {
            return fmt::format_to(ctx.out(), "{}", toml::table(val));
        }
        else {
            using map_t = hera::input_map::map_t;
            return formatter<map_t>::format(*val, ctx);
        }
    }
};

template<>
struct fmt::formatter<hera::input_atlas> : hera::format_parser<> {
    auto format(const hera::input_atlas& val, auto& ctx) const
    {
        auto out = ctx.out();
        if (alternate) {
            return fmt::format_to(out, "{}", toml::table(val));
        }
        else {
            auto&& ms =
                val.map_stack |
                hera::views::transform(
                    [](auto&& p) -> hera::pair<const hera::input_map&, bool> {
                        return {*p.first, p.second};
                    });
            return fmt::format_to(out, "input_atlas:\n\tstack:{}\n\tmaps:{}",
                                  ms, val.atlas);
        }
    }
};

#endif
