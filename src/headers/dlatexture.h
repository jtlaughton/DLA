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
    float weightedHeight = 0.0f;
    float height = 0.0f;
    bool placed = false;
    int x = 0, y = 0;
    std::vector<int> downStream;
    bool startingHeight = false;

    void Place(const int connection, const int newId, int newX, int newY, int position, std::vector<ConnectedPoint>& connectedPoints) {
        placed = true;
        height = 1.0f;
        weightedHeight = 1.0f - (1.0f / (1.0f + height));
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

        float newHeight = height + 1.0f;

        if (newHeight <= neighborHeight) {
            return;
        }

        connectedPoints[indexConnection].height = newHeight;
        connectedPoints[indexConnection].weightedHeight = 1.0f - (1.0f / (1.0f + newHeight));
        connectedPoints[indexConnection].startingHeight = false;

        if (connectedPoints[indexConnection].height >= heightAccumulator) {
            heightAccumulator = connectedPoints[indexConnection].height;
        }

        connectedPoints[indexConnection].TryIncreaseHeight(connectedPoints, heightAccumulator);
    }
};

class DLATexture {
public:
    unsigned int blurredWidth = 8;
    unsigned int blurredHeight = 8;
    unsigned int width = 8;
    unsigned int height = 8;
    DLATexture();
    std::string GetBlurredByteStream() const;
    std::string GetSharpByteStream() const;
    bool RunIteration();
    void Reset();
    void BiLinearInterpolationBy2();
    void Blur();
    void SharpUpscale();
    void CombineBlurredAndSharp();
    void RunSequence();
private:
    int currentPlaced = 0;
    float maxHeight = 1.0f;
    unsigned int currentIteration = 0;
    unsigned int _randomMove(unsigned int& x, unsigned int& y);
    int _nearbyAssigned(int x, int y);
    unsigned int maxIterations = 16;
    std::vector<ConnectedPoint> sharpPoints;
    std::vector<ConnectedPoint> blurredPoints;
    std::mt19937 gen;
    std::uniform_int_distribution<unsigned> distribX;
    std::uniform_int_distribution<unsigned> distribY;
    std::uniform_int_distribution<unsigned> distribDir;
    std::string GetByteStream(const std::vector<ConnectedPoint>& vector) const;
    void RunAllIterations();
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