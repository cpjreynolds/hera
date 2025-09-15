#version 410 core
#include "common.glsl"

out vec4 gl_Position;

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;

out VERT_OUT
{
    vec2 texcoord;
    vec3 normal;
    vec3 frag_pos;
}
outv;

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    outv.frag_pos = vec3(model * vec4(a_pos, 1.0));
    outv.texcoord = a_texcoord;
    outv.normal = vec3(model * vec4(a_normal, 0.0));
}
