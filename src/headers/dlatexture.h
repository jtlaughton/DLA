#pragma once

#include <random>
#include <string>
#include <vector>
#include <glad/glad.h>
#include "../headers/FastRandomHashMap.h"

class DLATexture {
public:
    const unsigned int WIDTH = 100;
    const unsigned int HEIGHT = 100;
    DLATexture();
    std::string GetByteStream();
    bool RunIteration();
    void Reset();
private:
    unsigned int currentIteration = 0;
    unsigned int _randomMove(unsigned int& x, unsigned int& y);
    bool _nearbyAssigned(unsigned int x, unsigned int y);
    const unsigned int maxIterations = 1000;
    std::vector<bool> points;
    std::mt19937 gen;
    std::uniform_int_distribution<unsigned> distribX;
    std::uniform_int_distribution<unsigned> distribY;
    std::uniform_int_distribution<unsigned> distribDir;
};
