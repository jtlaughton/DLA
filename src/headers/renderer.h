#pragma once

#include "../headers/Shader.h"
#include "../headers/dlatexture.h"

#include <chrono>
#include <fstream>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>

inline void framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
    glViewport(0, 0, width, height);
}

inline void processInput(GLFWwindow* window) {
     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
         glfwSetWindowShouldClose(window, GLFW_TRUE);
     }
}

class Renderer {
public:
    const int WINDOW_HEIGHT = 1024;
    const int WINDOW_WIDTH = 1024;

    Renderer();
    [[nodiscard]] int Init();
    [[nodiscard]] bool Running() const;
    void Update();
    void End() const;
private:
    int currentIteration = 0;
    const int iterations = 10;
    DLATexture generator;
    GLFWwindow* window;
    bool running = false;
    bool iterationsFinished = false;
    bool scalingApplied = false;
    bool printed = false;
    void Draw();
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    GLuint texture1;
    Shader* shader;
    std::chrono::time_point<std::chrono::system_clock> start;
    std::vector<std::chrono::milliseconds> timeStamps;
};

constexpr GLfloat vertices[] = {
    // Verts                //color             //tex coords
    1.0f,  1.0f, 0.0f,      1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    1.0f, -1.0f, 0.0f,      0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -1.0f,  1.0f, 0.0f,     0.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
};
const GLuint indices[] = {
    0, 1, 3,
    1, 2, 3
};
