#include <stdio.h>

#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define INFO_BUFFER_SIZE 512

const char *vertexShaderSrc = "#version 410 core\n"
    "layout (location = 0) in vec3 a_pos;"
    "void main() {\n"
    "    gl_Position = vec4(a_pos.x, a_pos.y, a_pos.z, 1.0f);\n"
    "}\0";

const char *fragShaderSrc = "#version 410 core\n"
    "out vec4 o_colour;"
    "void main() {\n"
    "    o_colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\0";

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
    char infoLogBuffer[512];
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLogBuffer);
        printf("Shader link failure\nError: %s\n", infoLogBuffer); 
    }
    return program;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
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

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);
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

    // compile shaders
    unsigned int vertexShader = 0;
    unsigned int fragShader = 0;
    compileShader(&vertexShader, GL_VERTEX_SHADER, vertexShaderSrc);
    compileShader(&fragShader, GL_FRAGMENT_SHADER, fragShaderSrc);

    // link shaders
    unsigned int shaderProgram = linkShaders(vertexShader, fragShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    // Hello triangle
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
    };

    // buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Render
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    printf("exiting\n");
    return 0;
}
