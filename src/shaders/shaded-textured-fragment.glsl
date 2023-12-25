#version 410 core

uniform vec4 u_light_pos;  // (x, y, z, 1): world pos | (x, y, z, 0): (normalised) source vector
uniform vec3 u_view_pos;   // world
uniform float u_specularity;

uniform float u_diffuse_intensity;
uniform float u_ambient_intensity;
uniform float u_specular_intensity;

uniform sampler2D u_diffuse_texture;
uniform sampler2D u_specular_texture;
uniform sampler2D u_shadow_map;

in vec3 v_pos;       // world
in vec3 v_normal;    // world
in vec2 v_texcoord;  // UV
in vec4 v_pos_light_space;

out vec4 o_colour;

float shadow_calc() {
    vec3 projection_coords = v_pos_light_space.xyz / v_pos_light_space.w;
    projection_coords = projection_coords * 0.5 + 0.5;

    float closest = texture(u_shadow_map, projection_coords.xy).r;
    float current = projection_coords.z;
    if (current > closest) {
        return 0.25;
    } else {
        return 1.0;
    }
}

void main() {
    vec3 diffuse_intensity = vec3(u_diffuse_intensity);
    vec3 ambient_intensity = vec3(u_ambient_intensity);
    vec3 specular_intensity = vec3(u_specular_intensity);

    vec3 normal = normalize(v_normal);

    vec3 light_direction;
    if (u_light_pos[3] == 0) {
        // directional light
        light_direction = u_light_pos.xyz;
    } else {
        // point light
        light_direction = normalize(u_light_pos.xyz - v_pos);
    }

    vec3 view_direction = normalize(u_view_pos - v_pos);  // perspective

    float dot_product = dot(light_direction, normal);

    vec3 diffuse = diffuse_intensity * max(0, dot_product);
    vec3 specular = vec3(0);

    if (dot_product > 0) {
        vec3 reflection = reflect(-light_direction, normal);
        float phong_angle = pow(max(0, dot(reflection, view_direction)), u_specularity);
        vec3 specular_tex_value = vec3(texture(u_specular_texture, v_texcoord));
        specular = specular_intensity * phong_angle * specular_tex_value;
    }

    // Shadows
    float shadow = shadow_calc();
    float shadow_norm = (shadow-0.25)/(1-0.25);
    vec3 intensity = ambient_intensity + (diffuse * shadow) + (specular * shadow_norm);

    o_colour = texture(u_diffuse_texture, v_texcoord) * vec4(intensity, 1);
    // o_colour = vec4(intensity, 1);
    // o_colour = vec4(specular, 1);
    // o_colour = vec4(v_normal, 1);
}
