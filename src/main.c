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

#define ROTATION_DEGREES_PER_SEC 20.0f

#define N_MODELS 1

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

    window = glfwCreateWindow(screen_width, screen_height, "Hello Rat", NULL, NULL);
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

    // Load model(s)
    scene_object loaded_models[N_MODELS];
    loaded_models[0] = scn_obj_load("models/potkan.obj", SCN_OBJ_IMPORT_UNIT_RESCALE
                                                          | SCN_OBJ_IMPORT_CENTRED);
    scn_obj_load_texture(&loaded_models[0], "models/potkan.png");

    // Create pivot(s) for model(s)
    scene_object pivots[N_MODELS];
    for (int i = 0; i < N_MODELS; i++) {
        pivots[i] = scn_obj_pivot();
        loaded_models[i].parent = &pivots[i];
    }

    mat4_rotate_x(&loaded_models[0].model_matrix, degrees_to_radians(-90.0f));
    mat4_translate(&loaded_models[0].model_matrix, -0.3845f, 0.0f, 0.0f);
    mat4_translate(&pivots[0].model_matrix, 0.0f, -1.0f, 0.0f);

    loaded_models[0].shader_prog = shader_create_program("src/shaders/shaded-textured-vertex.glsl",
                                                         "src/shaders/shaded-textured-fragment.glsl");

    // Light (point)
    // vec4 point_light_pos = vec4_zero();
    // point_light_pos.x = -1.0f;
    // point_light_pos.y = 2.0f;
    // point_light_pos.z = 2.0f;
    // point_light_pos.w = 1.0f;

    // Light (directional source vector)
    vec4 directional_light = vec4_zero();
    directional_light.x = -0.125f;
    directional_light.y = 1.0f;
    directional_light.z = 1.0f;

    // Cameras
    view_matrix = mat4_identity();
    mat4_translate(&view_matrix, 0.0f, 0.0f, -2.0f);

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

        float rotation_amount = degrees_to_radians(ROTATION_DEGREES_PER_SEC * frame_time);
        for (int i = 0; i < N_MODELS; i++) {
            shader_program shader = loaded_models[i].shader_prog;
            glUseProgram(shader);

            // shader_set_uniform_vec4(shader, "u_light_pos", point_light_pos);
            shader_set_uniform_vec4(shader, "u_light_pos", directional_light);

            // -, view matrix faces backwards
            int u_view_pos_loc = glGetUniformLocation(shader, "u_view_pos");
            glUniform3f(u_view_pos_loc, -view_matrix.data[3], -view_matrix.data[7], -view_matrix.data[11]);

            shader_set_uniform_float(shader, "u_specularity", 4.0f);
            shader_set_uniform_float(shader, "u_diffuse_intensity", 0.8f);
            shader_set_uniform_float(shader, "u_ambient_intensity", 0.1f);
            shader_set_uniform_float(shader, "u_specular_intensity", 0.2f);

            // transform - spin pivot(s) of the loaded model(s)
            mat4_rotate_y(&pivots[i].model_matrix, rotation_amount);

            // Calculate and pass in mvp matrix
            mat4 mvp = scn_obj_mvp(&loaded_models[i], view_matrix, projection_matrix);
            shader_set_uniform_mat4(shader, "u_mvp", mvp);

            mat4 model_to_world = scn_obj_world_mat(&loaded_models[i]);
            shader_set_uniform_mat4(shader, "u_model", model_to_world);

            // Draw
            glBindTexture(GL_TEXTURE_2D, loaded_models[i].texture);
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

    for (int i = 0; i < N_MODELS; i++) {
        glDeleteProgram(loaded_models[i].shader_prog);
        scn_obj_clean(&loaded_models[i]);
        scn_obj_clean(&pivots[i]);
    }

    printf("exiting\n");
    return 0;
}
