#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>

class DLATexture {
public:
    const unsigned int WIDTH = 20;
    const unsigned int HEIGHT = 20;
    DLATexture();
    std::string GetByteStream();
    void RunIteration();
private:
    const unsigned int iterations = 20;
    std::vector<bool> points;
};
