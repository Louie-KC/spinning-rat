#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "matrix.h"
#include "vector.h"
#include "util.h"
#include "scene-object.h"
#include "shader.h"

#define INFO_BUFFER_SIZE 512
#define SHADER_SOURCE_MAX_LEN 4096

#define ROTATION_DEGREES_PER_SEC 90.0f

#define N_MODELS 2

int screen_width = 640;
int screen_height = 480;

// Camera
mat4 projection_matrix;
mat4 view_matrix;

void process_input(GLFWwindow* window, double frame_time) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W)) {
        mat4_translate(&view_matrix, 0.0f, 0.0f, 1.0f * frame_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
        mat4_translate(&view_matrix, 0.0f, 0.0f, -1.0f * frame_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        mat4_translate(&view_matrix, 1.0f * frame_time, 0.0f, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        mat4_translate(&view_matrix, -1.0f * frame_time, 0.0f, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_E)) {
        mat4_translate(&view_matrix, 0.0f, -1.0f * frame_time, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_Q)) {
        mat4_translate(&view_matrix, 0.0f, 1.0f * frame_time, 0.0f);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    screen_width = width;
    screen_height = height;
    camera_persp(&projection_matrix, degrees_to_radians(45.0f),
                 (float) screen_width / (float) screen_height, 0.1f, 100.0f);
    glViewport(0, 0, width, height);
}

int main(void) {
    GLFWwindow* window;
    
    if (!glfwInit()) {
        printf("glfwInit failure\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(screen_width, screen_height, "Hello World", NULL, NULL);
    if (!window) {
        printf("glfwCreateWindow failure\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad init
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("GLAD init failure\n");
        glfwTerminate();
        return -1;
    }

    // read and compile shaders
    shader_program shader_program = shader_create_program("src/shaders/shaded-vertex.glsl",
                                                          "src/shaders/shaded-fragment.glsl");

    // Load model(s)
    scene_object loaded_models[N_MODELS];
    load_model("models/teapot.obj", &loaded_models[0].vertex_buffer,
               &loaded_models[0].index_buffer, &loaded_models[0].normals_buffer,
               &loaded_models[0].vertex_buffer_len, &loaded_models[0].index_buffer_len,
               UTIL_PROCESS_SCALE_MODEL | UTIL_PROCESS_FLIP_NORMALS, 1);
    scn_obj_init(&loaded_models[0]);

    load_model("models/cube.obj", &loaded_models[1].vertex_buffer,
               &loaded_models[1].index_buffer, &loaded_models[1].normals_buffer,
               &loaded_models[1].vertex_buffer_len, &loaded_models[1].index_buffer_len,
               0, 0);
    scn_obj_init(&loaded_models[1]);

    mat4_translate(&loaded_models[0].model_matrix, 0.0f, -0.5f, 0.0f);
    mat4_translate(&loaded_models[1].model_matrix, 1.0f, 1.0f, -2.0f);

    // Light (point)
    vec3 point_light_pos = vec3_zero();
    point_light_pos.x = -1.0f;
    point_light_pos.y = 2.0f;
    point_light_pos.z = 2.0f;

    // Cameras
    view_matrix = mat4_identity();
    mat4_translate(&view_matrix, 0.0f, 0.0f, -3.0f);

    projection_matrix = mat4_zero();
    camera_persp(&projection_matrix, degrees_to_radians(45.0f), (float) screen_width / (float) screen_height, 0.1f, 100.0f);

    // Time
    double last_frame = glfwGetTime();
    double frame_time = 0.0;

    // Render
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        process_input(window, frame_time);

        glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);

        shader_set_uniform_vec3(shader_program, "u_light_pos", point_light_pos);

        // -, view matrix faces backwards
        int u_view_pos_loc = glGetUniformLocation(shader_program, "u_view_pos");
        glUniform3f(u_view_pos_loc, -view_matrix.data[3], -view_matrix.data[7], -view_matrix.data[11]);

        shader_set_uniform_float(shader_program, "u_specularity", 32.0f);
        shader_set_uniform_float(shader_program, "u_diffuse_intensity", 0.8f);
        shader_set_uniform_float(shader_program, "u_ambient_intensity", 0.1f);
        shader_set_uniform_float(shader_program, "u_specular_intensity", 0.2f);

        float rotation_amount = degrees_to_radians(ROTATION_DEGREES_PER_SEC * frame_time);
        for (int i = 0; i < N_MODELS; i++) {
            // transform - spin the loaded model
            mat4_rotate_y(&loaded_models[i].model_matrix, rotation_amount);

            // Calculate and pass in mvp matrix
            mat4 mvp = scn_obj_mvp(&loaded_models[i], view_matrix, projection_matrix);
            shader_set_uniform_mat4(shader_program, "u_mvp", mvp);

            shader_set_uniform_mat4(shader_program, "u_model", loaded_models[i].model_matrix);

            // Draw
            glBindVertexArray(loaded_models[i].VAO);
            glDrawElements(GL_TRIANGLES, loaded_models[i].index_buffer_len, GL_UNSIGNED_INT, 0);
            // glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }

        // Buffer swap and IO
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Time
        frame_time = glfwGetTime() - last_frame;
        last_frame = glfwGetTime();
    }

    glDeleteProgram(shader_program);
    for (int i = 0; i < N_MODELS; i++) {
        scn_obj_clean(&loaded_models[i]);
    }

    printf("exiting\n");
    return 0;
}
