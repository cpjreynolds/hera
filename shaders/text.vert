#version 410 core
#include "common.glsl"

out vec4 gl_Position;

layout(location = 0) in vec4 vertex; // {posx, posy, tex_x, tex_y}

uniform vec2 origin;

out vec2 tex_coord;

void main()
{
    gl_Position = vec4(origin, 0.0, 0.0) + ortho * vec4(vertex.xy, 0.0, 1.0);
    tex_coord = vertex.zw;
}
