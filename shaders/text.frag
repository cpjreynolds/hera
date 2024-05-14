#version 410 core
#include "common.glsl"

in vec2 tex_coord;

out vec4 f_color;

uniform sampler2DArray glyph;
uniform vec3 text_color;
uniform int chidx;

void main()
{
    float alpha = texture(glyph, vec3(tex_coord, chidx)).r;
    f_color = vec4(1.0, 1.0, 1.0, alpha) * vec4(text_color, 1.0);
    // f_color = vec4(1.0, 0.0, 0.0, 1.0);
}
