#pragma once

#include "../headers/Shader.h"
#include "../headers/dlatexture.h"

#include <fstream>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
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
    void End();
private:
    DLATexture generator;
    GLFWwindow* window;
    bool running = false;
    void Draw() const;
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    GLuint texture1;
    Shader* shader;
};

const GLfloat vertices[] = {
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
