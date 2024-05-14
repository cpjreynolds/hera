#version 410 core
#include "common.glsl"

out vec4 frag_color;

in VERT_OUT
{
    vec2 texcoord;
    vec3 normal;
    vec3 frag_pos;
}
inv;
uniform float light_ambient;
uniform vec3 light_color;
uniform vec3 light_pos;

uniform sampler2D a_texture;
uniform sampler2D spec_map;

const float spec_strength = 0.5;

void main()
{
    // ambient
    vec3 ambient = light_ambient * texture(a_texture, inv.texcoord).rgb;

    // diffuse
    vec3 norm = normalize(inv.normal);
    vec3 light_dir = normalize(light_pos - inv.frag_pos);
    float theta = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = theta * light_color * texture(a_texture, inv.texcoord).rgb;

    // specular
    vec3 view_dir = normalize(view_pos - inv.frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = spec * light_color * texture(spec_map, inv.texcoord).rgb;

    vec3 light_total = ambient + diffuse + specular;

    frag_color = vec4(light_total, 1.0);
}
