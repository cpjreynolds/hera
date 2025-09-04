#version 410 core
#include "common.glsl"

out vec4 frag_color;

struct Material {
    sampler2D diff;
    sampler2D spec;
    float shine;
};

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

uniform sampler2D t_diffuse;
uniform sampler2D t_specular;
uniform float shine;

// uniform Material material;

void main()
{
    // ambient
    vec3 ambient = light_ambient * texture(t_diffuse, inv.texcoord).rgb;

    // diffuse
    vec3 norm = normalize(inv.normal);
    vec3 light_dir = normalize(light_pos - inv.frag_pos);
    float theta = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = theta * light_color * texture(t_diffuse, inv.texcoord).rgb;

    // specular
    vec3 view_dir = normalize(view_pos - inv.frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shine);
    vec3 specular = spec * light_color * texture(t_specular, inv.texcoord).rgb;

    vec3 light_total = ambient + diffuse + specular;

    frag_color = vec4(light_total, 1.0);
}
