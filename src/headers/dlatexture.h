#pragma once

#include <iostream>
#include <cmath>
#include <random>
#include <string>
#include <vector>
#include <glad/glad.h>
#include "../headers/FastRandomHashMap.h"

struct ConnectedPoint {
    int indexConnection = -1;
    float height = 0.0f;
    bool placed = false;

    void Place(int connection) {
        placed = true;
        height = 1.0f;
        indexConnection = connection;
    }

    void TryIncreaseHeight(std::vector<ConnectedPoint>& connectedPoints, float& heightAccumulator) {
        if (indexConnection == -1) {
            return;
        }

        float neighborHeight = connectedPoints[indexConnection].height;

        float newHeight = height + (1.0f - (1.0f / (1.0f + height)));

        if (newHeight <= neighborHeight) {
            return;
        }

        connectedPoints[indexConnection].height = newHeight;

        if (connectedPoints[indexConnection].height >= heightAccumulator) {
            heightAccumulator = connectedPoints[indexConnection].height;
        }

        connectedPoints[indexConnection].TryIncreaseHeight(connectedPoints, heightAccumulator);
    }
};

class DLATexture {
public:
    unsigned int width = 128;
    unsigned int height = 128;
    DLATexture();
    std::string GetByteStream();
    bool RunIteration();
    void Reset();
    void BiLinearInterpolationBy2();
    void Blur();
private:
    float maxHeight = 1.0f;
    unsigned int currentIteration = 0;
    unsigned int _randomMove(unsigned int& x, unsigned int& y);
    int _nearbyAssigned(int x, int y);
    const unsigned int maxIterations = 4000;
    std::vector<ConnectedPoint> points;
    std::mt19937 gen;
    std::uniform_int_distribution<unsigned> distribX;
    std::uniform_int_distribution<unsigned> distribY;
    std::uniform_int_distribution<unsigned> distribDir;
};

const float CENTER_SAMPLE = 1.0f / 3.0f;
const float LEFT_SAMPLE = 1.0f / 6.0f;
const float DOWN_SAMPLE = 1.0f / 6.0f;
const float RIGHT_SAMPLE = 1.0f / 6.0f;
const float UP_SAMPLE = 1.0f / 6.0f;
