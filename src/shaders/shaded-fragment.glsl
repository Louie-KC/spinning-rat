#version 410 core

uniform vec3 u_light_pos;  // world
uniform vec3 u_view_pos;   // world
uniform float u_specularity;

uniform float u_diffuse_intensity;
uniform float u_ambient_intensity;
uniform float u_specular_intensity;

in vec3 v_pos;
in vec3 v_normal;

out vec4 o_colour;

void main() {
    vec3 diffuse_intensity = vec3(u_diffuse_intensity);
    vec3 ambient_intensity = vec3(u_ambient_intensity);
    vec3 specular_intensity = vec3(u_specular_intensity);

    vec3 normal = normalize(v_normal);
    vec3 light_direction = normalize(u_light_pos - v_pos);
    vec3 view_direction = normalize(u_view_pos - v_pos);  // perspective

    float dot_product = dot(light_direction, normal);

    vec3 diffuse = diffuse_intensity * max(0, dot_product);
    vec3 specular = vec3(0);

    if (dot_product > 0) {
        vec3 reflection = reflect(-light_direction, normal);
        float phong_angle = pow(max(0, dot(reflection, view_direction)), u_specularity);
        specular = specular_intensity * phong_angle;
    }

    vec3 intensity = ambient_intensity + diffuse + specular;
    o_colour = vec4(intensity, 1);
    // o_colour = vec4(specular, 1);
    // o_colour = vec4(v_normal, 1);
}
