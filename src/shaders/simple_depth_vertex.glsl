#version 410 core

uniform mat4 u_light_camera_vp;
uniform mat4 u_model;

layout (location = 0) in vec3 a_pos;

void main() {
    gl_Position = u_light_camera_vp * u_model * vec4(a_pos, 1.0);
}