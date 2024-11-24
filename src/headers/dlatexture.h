#pragma once

#include <unordered_set>
#include <iostream>
#include <cmath>
#include <random>
#include <string>
#include <vector>
#include <glad/glad.h>
#include "../headers/FastRandomHashMap.h"

struct ConnectedPoint {
    int id = -1;
    int indexConnection = -1;
    float height = 0.0f;
    bool placed = false;
    int x = 0, y = 0;
    std::vector<int> downStream;
    bool startingHeight = false;

    void Place(const int connection, const int newId, int newX, int newY, int position, std::vector<ConnectedPoint>& connectedPoints) {
        placed = true;
        height = 1.0f;
        indexConnection = connection;
        id = newId;
        x = newX;
        y = newY;
        startingHeight = true;

        if (indexConnection >= 0) {
            connectedPoints[indexConnection].downStream.push_back(position);
        }
    }

    void TryIncreaseHeight(std::vector<ConnectedPoint>& connectedPoints, float& heightAccumulator) {
        if (indexConnection < 0) {
            return;
        }

        float neighborHeight = connectedPoints[indexConnection].height;

        float newHeight = height + (1.0f - (1.0f / (1.0f + height)));

        if (newHeight <= neighborHeight) {
            return;
        }

        connectedPoints[indexConnection].height = newHeight;
        connectedPoints[indexConnection].startingHeight = false;

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
    void SharpUpscale();
private:
    int currentPlaced = 0;
    float maxHeight = 1.0f;
    unsigned int currentIteration = 0;
    unsigned int _randomMove(unsigned int& x, unsigned int& y);
    int _nearbyAssigned(int x, int y);
    const unsigned int maxIterations = 1000;
    std::vector<ConnectedPoint> sharpPoints;
    std::vector<ConnectedPoint> blurredPoints;
    std::mt19937 gen;
    std::uniform_int_distribution<unsigned> distribX;
    std::uniform_int_distribution<unsigned> distribY;
    std::uniform_int_distribution<unsigned> distribDir;
};

constexpr float CENTER_SAMPLE = 1.0f / 3.0f;
constexpr float LEFT_SAMPLE = 1.0f / 6.0f;
constexpr float DOWN_SAMPLE = 1.0f / 6.0f;
constexpr float RIGHT_SAMPLE = 1.0f / 6.0f;
constexpr float UP_SAMPLE = 1.0f / 6.0f;

// order of operations
// 1. run starting iterations
// 2. create blurred upscale
// 3. create sharp upscale
// 4. run dla on sharp upscale
// 5. add sharp details to blurred upscale
// 6. repeat 2-5 for n iterations