#version 410 core

layout (location = 0) in vec3 a_pos;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(a_pos, 1.0f);
}
