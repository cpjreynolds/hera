#version 410 core
#include "common.glsl"

out vec4 frag_color;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_POINT_LIGHTS 4

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shine;
};

in VERT_OUT
{
    vec2 texcoord;
    vec3 normal;
    vec3 frag_pos;
}
inv;

uniform Material material;

uniform PointLight point_lights[MAX_POINT_LIGHTS];

uniform DirLight dir_light;

uniform int n_point_lights;

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_dir, Material m)
{
    vec3 light_dir = normalize(-light.direction);
    // diffuse
    float diff = max(dot(normal, light_dir), 0.0);
    // specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), m.shine);

    // sample & combine
    vec3 ambient = light.ambient * texture(m.diffuse, inv.texcoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(m.diffuse, inv.texcoord).rgb;
    vec3 specular =
        light.specular * spec * texture(m.specular, inv.texcoord).rgb;
    return (ambient + diffuse + specular);
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 view_dir, Material m)
{
    vec3 light_dir = normalize(light.position - inv.frag_pos);
    // diffuse
    float diff = max(dot(normal, light_dir), 0.0);
    // specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), m.shine);
    // attenuation
    float dist = length(light.position - inv.frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * dist +
                               light.quadratic * (dist * dist));

    // sample & combine
    vec3 ambient = light.ambient * texture(m.diffuse, inv.texcoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(m.diffuse, inv.texcoord).rgb;
    vec3 specular =
        light.specular * spec * texture(m.specular, inv.texcoord).rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 norm = normalize(inv.normal);
    vec3 view_dir = normalize(view_pos - inv.frag_pos);

    vec3 light_total = calc_dir_light(dir_light, norm, view_dir, material);

    for (int i = 0; i < n_point_lights; ++i) {
        light_total +=
            calc_point_light(point_lights[i], norm, view_dir, material);
    }

    frag_color = vec4(light_total, 1.0);
}
