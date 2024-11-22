#pragma once

#include <random>
#include <string>
#include <vector>
#include <glad/glad.h>

class DLATexture {
public:
    const unsigned int WIDTH = 400;
    const unsigned int HEIGHT = 400;
    DLATexture();
    std::string GetByteStream();
    bool RunIteration();
private:
    unsigned int currentIteration = 0;
    unsigned int _randomMove(unsigned int& x, unsigned int& y);
    bool _nearbyAssigned(unsigned int x, unsigned int y);
    const unsigned int maxIterations = 5000;
    std::vector<bool> points;
    std::mt19937 gen;
    std::uniform_int_distribution<unsigned> distribX;
    std::uniform_int_distribution<unsigned> distribY;
    std::uniform_int_distribution<unsigned> distribDir;
};
