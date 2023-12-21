#include "shader.h"

shader_program shader_create_program(const char* vertex_path, const char* frag_path) {
    shader_program program;
    unsigned int vertex_shader;
    unsigned int frag_shader;
    char vertex_shader_source[SHADER_SOURCE_MAX_LEN];
    char frag_shader_source[SHADER_SOURCE_MAX_LEN];

    if (!read_file_source(vertex_path, vertex_shader_source, SHADER_SOURCE_MAX_LEN)) {
        printf("failed to read vertex shader source\n");           
    }
    if (!read_file_source(frag_path, frag_shader_source, SHADER_SOURCE_MAX_LEN)) {
        printf("failed to read fragment shader source\n");
    }

    shader_compile(&vertex_shader, GL_VERTEX_SHADER, vertex_shader_source);
    shader_compile(&frag_shader, GL_FRAGMENT_SHADER, frag_shader_source);

    program = glCreateProgram();
    shader_link(program, vertex_shader, frag_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);

    return program;
}

int shader_compile(unsigned int* shader, int type, const char* source) {
    int compile_success;
    char compile_log_buffer[SHADER_INFO_BUFFER_SIZE];
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compile_success);
    if (!compile_success) {
        glGetShaderInfoLog(*shader, SHADER_INFO_BUFFER_SIZE, NULL, compile_log_buffer);
        printf("Shader compile failure: \n%s\n%s\n", source, compile_log_buffer);
    }
    return compile_success;
}

shader_program shader_link(shader_program program, unsigned int vertex, unsigned int fragment) {
    int link_success = 0;
    char link_log_buffer[SHADER_INFO_BUFFER_SIZE];
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_success);
    if (!link_success) {
        glGetProgramInfoLog(program, SHADER_INFO_BUFFER_SIZE, NULL, link_log_buffer);
        printf("Shader link failure - %s\n", link_log_buffer); 
    }
    return link_success;
}

void shader_set_uniform_float(shader_program program, const char* name, float value) {
    int loc = glGetUniformLocation(program, name);
    glUniform1f(loc, value);
}

void shader_set_uniform_vec2(shader_program program, const char* name, vec2 vec) {
    int loc = glGetUniformLocation(program, name);
    glUniform2fv(loc, 1, (GLfloat *) &vec);
}

void shader_set_uniform_vec3(shader_program program, const char* name, vec3 vec) {
    int loc = glGetUniformLocation(program, name);
    glUniform3fv(loc, 1, (GLfloat *) &vec);
}

void shader_set_uniform_vec4(shader_program program, const char* name, vec4 vec) {
    int loc = glGetUniformLocation(program, name);
    glUniform4fv(loc, 1, (GLfloat *) &vec);
}

void shader_set_uniform_mat3(shader_program program, const char* name, mat3 mat) {
    int loc = glGetUniformLocation(program, name);
    glUniformMatrix3fv(loc, 1, GL_TRUE, (GLfloat *) &mat.data);
}

void shader_set_uniform_mat4(shader_program program, const char* name, mat4 mat) {
    int loc = glGetUniformLocation(program, name);
    glUniformMatrix4fv(loc, 1, GL_TRUE, (GLfloat *) &mat.data);
}

void shader_set_uniform_tex_i(shader_program program, const char* name, unsigned int tex_id) {
    int loc = glGetUniformLocation(program, name);
    glUniform1i(loc, tex_id);
}
