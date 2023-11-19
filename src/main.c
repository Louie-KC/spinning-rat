#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "matrix.h"
// #include "vector.c"

#include "util.c"

#define INFO_BUFFER_SIZE 512
#define SHADER_SOURCE_MAX_LEN 4096

#define ROTATION_DEGREES_PER_SEC 90.0f

int screen_width = 640;
int screen_height = 480;

// Camera
mat4 projection_matrix;

int compileShader(unsigned int* shader, int type, const char *source) {
    int status;
    char infoLogBuffer[INFO_BUFFER_SIZE];
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(*shader, INFO_BUFFER_SIZE, NULL, infoLogBuffer);
        printf("Shader compile failure: \n%s\nError: %s\n", source, infoLogBuffer);
    }
    return status;
}

// return: shader program
unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader) {
    int success = 0;
    char infoLogBuffer[INFO_BUFFER_SIZE];
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, INFO_BUFFER_SIZE, NULL, infoLogBuffer);
        printf("Shader link failure\nError: %s\n", infoLogBuffer); 
    }
    return program;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    screen_width = width;
    screen_height = height;
    camera_persp(&projection_matrix, degrees_to_radians(45.0f), (float) screen_width / (float) screen_height, 0.1f, 100.0f);
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
    unsigned int vertexShader = 0;
    unsigned int fragShader = 0;

    char vertexShaderSource[SHADER_SOURCE_MAX_LEN];
    char fragmentShaderSource[SHADER_SOURCE_MAX_LEN];
    
    if (!read_file_source("src/shaders/basic-vertex.glsl", vertexShaderSource)) {
        printf("failed to read vertex shader source\n");           
    }
    if (!read_file_source("src/shaders/basic-fragment.glsl", fragmentShaderSource)) {
        printf("failed to read fragment shader source\n");
    }

    compileShader(&vertexShader, GL_VERTEX_SHADER, vertexShaderSource);
    compileShader(&fragShader, GL_FRAGMENT_SHADER, fragmentShaderSource);

    // link shaders
    unsigned int shaderProgram = linkShaders(vertexShader, fragShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    // Load model: Cube
    float *vertices;
    unsigned int *indices;
    int n_vertices;
    int n_indices;
    load_model("models/cube.obj", &vertices, &indices, &n_vertices, &n_indices);

    // printf("n_vertices: %d\nn_indices: %d\n", n_vertices, n_indices);
    // printf("\n\n");
    // for (int i = 0; i < 24; ++i) {
    //     if (i % 3 == 0) {
    //         printf("\n");
    //     }
    //     printf("%f ", vertices[i]);
    // }
    // printf("\n");
    // for (int i = 0; i < 36; ++i) {
    //     printf("%d ", indices[i]);
    //     if ((i + 1) % 3 == 0) {
    //         printf("\n");
    //     }
    // }
    // printf("\n");

    // Square transform setup
    mat4 square_model = mat4_identity();
    // mat4_scale(&square_model, 0.5f, 0.5f, 0.5f);

    int square_model_loc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(square_model_loc, 1, GL_TRUE, (GLfloat *) &square_model.data);

    // buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * n_vertices, vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * n_indices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Cameras
    mat4 view = mat4_identity();
    mat4_translate(&view, 0.0f, 0.0f, -3.0f);

    projection_matrix = mat4_zero();
    camera_persp(&projection_matrix, degrees_to_radians(45.0f), (float) screen_width / (float) screen_height, 0.1f, 100.0f);
    
    int view_loc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(view_loc, 1, GL_TRUE, (GLfloat *) &view.data);
    
    int proj_loc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(proj_loc, 1, GL_TRUE, (GLfloat *) &projection_matrix.data);

    // Time
    double last_frame = glfwGetTime();
    double frame_time = 0.0;

    // Render
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Update matrices        
        // transform - spin the square
        mat4_rotate_y(&square_model, degrees_to_radians(ROTATION_DEGREES_PER_SEC * frame_time));
        square_model_loc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(square_model_loc, 1, GL_TRUE, (GLfloat *) &square_model.data);

        view_loc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(view_loc, 1, GL_TRUE, (GLfloat *) &view.data);

        proj_loc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(proj_loc, 1, GL_TRUE, (GLfloat *) &projection_matrix.data);

        // Draw
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, n_indices, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        // Buffer swap and IO
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Time
        frame_time = glfwGetTime() - last_frame;
        last_frame = glfwGetTime();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    
    free(vertices);
    free(indices);

    printf("exiting\n");
    return 0;
}
