#version 410 core

uniform mat4 u_mvp;    // model to NDC
uniform mat4 u_model;  // model to world

layout (location = 0) in vec3 a_pos;    // pos in model
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;  // UV

out vec3 v_pos;
out vec3 v_normal;
out vec2 v_texcoord;  // UV

void main() {
    v_pos = (u_model * vec4(a_pos, 1.0)).xyz;
    // v_normal = mat3(transpose(inverse(u_model))) * a_normal;
    v_normal = mat3(u_model) * a_normal;
    v_texcoord = a_texcoord;
    gl_Position = u_mvp * vec4(a_pos, 1.0);
}
