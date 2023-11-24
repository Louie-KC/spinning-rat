#version 410 core

uniform vec3 u_light_pos;  // world

in vec3 v_pos;
in vec3 v_normal;

out vec4 o_colour;

void main() {
    vec3 diffuse_intensity = vec3(1.0);
    vec3 ambient_intensity = vec3(0.1);

    vec3 normal = normalize(v_normal);
    vec3 light_direction = normalize(u_light_pos - v_pos);
    // vec3 light_direction = normalize(v_pos - u_light_pos);

    vec3 diffuse = diffuse_intensity * max(0, dot(light_direction, normal));
    vec3 intensity = ambient_intensity + diffuse;
    o_colour = vec4(intensity, 1);
    // o_colour = vec4(v_normal, 1);
}
