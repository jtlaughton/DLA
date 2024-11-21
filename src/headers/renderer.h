#pragma once

#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

inline void processInput(GLFWwindow* window) {
     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
         glfwSetWindowShouldClose(window, GLFW_TRUE);
     }
}

inline std::string LoadShaderAsString(const std::string& filename) {
    std::string result;

    std::string line;
    std::ifstream file(filename.c_str());

    if (file.is_open()) {
        while (std::getline(file, line)) {
            result += line + "\n";
        }
        file.close();
    }

    return result;
}

class Renderer {
public:
    const int WINDOW_HEIGHT = 600;
    const int WINDOW_WIDTH = 800;

    Renderer();
    [[nodiscard]] int Init();
    [[nodiscard]] bool Running() const;
    void Update();
    void End();
private:
    GLFWwindow* window;
    bool running = false;
    void Draw();
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    GLuint shaderProgram;
};

const GLfloat vertices[] = {
    0.5f,  0.5f, 0.0f,  // top right
    0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left
};
const GLuint indices[] = {
    0, 1, 3,
    1, 2, 3
};
