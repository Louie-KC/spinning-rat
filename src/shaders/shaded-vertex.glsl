#version 410 core

uniform mat4 u_mvp;    // model to NDC
uniform mat4 u_model;  // model to world

layout (location = 0) in vec3 a_pos;    // pos in model
layout (location = 1) in vec3 a_normal;

out vec3 v_pos;
out vec3 v_normal;

void main() {
    v_pos = (u_model * vec4(a_pos, 1.0)).xyz;
    // v_normal = mat3(transpose(inverse(u_model))) * a_normal;
    v_normal = mat3(u_model) * a_normal;
    gl_Position = u_mvp * vec4(a_pos, 1.0);
}
