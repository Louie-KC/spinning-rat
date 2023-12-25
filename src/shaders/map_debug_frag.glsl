#version 410 core

uniform sampler2D u_map;
uniform float u_near_plane;
uniform float u_far_plane;

in vec2 v_tex_coord;

out vec4 o_colour;

void main() {
    float depth = texture(u_map, v_tex_coord).r;
    o_colour = vec4(vec3(depth), 1.0);
}
