#pragma once

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

class Renderer {
    public:
        const int WINDOW_HEIGHT = 600;
        const int WINDOW_WIDTH = 800;

        Renderer();
        [[nodiscard]] int Init() const;
        bool Running() const;
        void Update();
        void End();
    private:
        GLFWwindow* window;
        bool running = false;
        void Draw();
};
