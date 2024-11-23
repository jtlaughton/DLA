//
// Created by jtlau on 11/21/2024.
//

#include "../headers/dlatexture.h"

DLATexture::DLATexture() {
    points = std::vector<ConnectedPoint>(width * height, ConnectedPoint());

    const unsigned int midX = width / 2;
    const unsigned int midY = height / 2;

    points[midX + midY * width].Place(-1);

    std::random_device rd;
    gen = std::mt19937(rd());
    distribDir = std::uniform_int_distribution<unsigned>(0, 3);
    distribX = std::uniform_int_distribution<unsigned>(0, width - 1);
    distribY = std::uniform_int_distribution<unsigned>(0, height - 1);
}

void DLATexture::Reset() {
    for (int i = 0; i < width * height; i++) {
        points[i] = ConnectedPoint();
    }

    const unsigned int midX = width / 2;
    const unsigned int midY = height / 2;

    points[midX + midY * width].Place(-1);

    currentIteration = 0;
}

std::string DLATexture::GetByteStream() {
    std::string byteString;

    for (const auto point : points) {
        if (point.placed) {
            float x = (point.height - 1.0f) / (maxHeight - 1.0f);
            float mappedHeight = 30.0f + (255.0f - 10.0f) * x;

            if (mappedHeight > 255.0f) {
                mappedHeight = 255.0f;
            }

            const char floored = static_cast<char>(floorf(mappedHeight));

            byteString += floored;
            byteString += floored;
            byteString += floored;

            // byteString += 0xFF;
            // byteString += 0xFF;
            // byteString += 0xFF;
        }
        else {
            // set to red
            byteString += 0xFF;
            byteString += 0x01;
            byteString += 0x01;
        }
    }

    return byteString;
}

unsigned int DLATexture::_randomMove(unsigned int& x, unsigned int& y) {
    switch (distribDir(gen)) {
        case 0:
            y += 1;
            if (y == height) {
                y -= 2;
            }
            break;
        case 1:
            y -= 1;
            if (y == -1) {
                y += 2;
            }
            break;
        case 2:
            x -= 1;
            if (x == -1) {
                x += 2;
            }
            break;
        case 3:
            x += 1;
            if (x == width) {
                x -= 2;
            }
            break;
        default:
            break;
    }

    return x + y * width;
}

int DLATexture::_nearbyAssigned(const int x, const int y) {
    const int left = (x - 1) + y * width;
    const int right = (x + 1) + y * width;
    const int up = x + (y - 1) * width;
    const int down = x + (y + 1) * width;

    if (x != 0 && points[left].placed)
        return left;

    if (x != width - 1 && points[right].placed)
        return right;

    if (y != 0 && points[up].placed)
        return up;

    if (y != height - 1 && points[down].placed)
        return down;

    return -1;
}

bool DLATexture::RunIteration() {
    if (currentIteration == maxIterations)
        return false;

    currentIteration++;

    unsigned int randx = distribX(gen);
    unsigned int randy = distribY(gen);

    ConnectedPoint current = points[randx + randy * width];

    while (current.placed) {
        current = points[_randomMove(randx, randy)];
    }

    int currentAssigned = _nearbyAssigned(randx, randy);

    while (currentAssigned == -1) {
        _randomMove(randx, randy);
        currentAssigned = _nearbyAssigned(randx, randy);
    }

    points[randx + randy * width].Place(currentAssigned);
    points[randx + randy * width].TryIncreaseHeight(points, maxHeight);

    return true;
}

void DLATexture::BiLinearInterpolationBy2() {
    const int oldWidth = static_cast<int>(width);
    const int oldHeight = static_cast<int>(height);
    const int newWidth = oldWidth * 2;
    const int newHeight = oldHeight * 2;

    auto newPoints = std::vector<ConnectedPoint>(newWidth * newHeight, ConnectedPoint());

    float w_scale = static_cast<float>(oldWidth) / static_cast<float>(newWidth);
    float h_scale = static_cast<float>(oldHeight) / static_cast<float>(newHeight);

    for (int i = 0; i < newWidth; i++) {
        for (int j = 0; j < newHeight; j++) {
            float x = static_cast<float>(i) * w_scale;
            float y = static_cast<float>(j) * h_scale;

            int x_floor = static_cast<int>(floorf(x));
            int x_ceil = std::min(oldWidth - 1, static_cast<int>(ceilf(x)));
            int y_floor = static_cast<int>(floorf(y));
            int y_ceil = std::min(oldHeight - 1, static_cast<int>(ceilf(y)));

            float height = 0.0f;
            bool placed = false;

            if (x_floor == x_ceil && y_floor == y_ceil) {
                height = points[x_floor + y_floor * width].height;
                placed = points[x_floor + y_floor * width].placed;
            }
            else if (x_ceil == x_floor) {
                float h1 = points[x_floor + y_floor * width].height;
                float h2 = points[x_floor + y_ceil * width].height;

                height = h1 * (static_cast<float>(y_ceil) - y) + h2 * (y - static_cast<float>(y_floor));

                bool p1 = points[x_floor + y_floor * width].placed;
                bool p2 = points[x_floor + y_ceil * width].placed;

                placed = p1 || p2;
            }
            else if (y_ceil == y_floor) {
                float h1 = points[x_floor + y_floor * width].height;
                float h2 = points[x_ceil + y_floor * width].height;

                height = h1 * (static_cast<float>(x_ceil) - x) + h2 * (x - static_cast<float>(x_floor));

                bool p1 = points[x_floor + y_floor * width].placed;
                bool p2 = points[x_ceil + y_floor * width].placed;

                placed = p1 || p2;
            }
            else {
                float h1 = points[x_floor + y_floor * width].height;
                float h2 = points[x_ceil + y_floor * width].height;
                float h3 = points[x_floor + y_ceil * width].height;
                float h4 = points[x_ceil + y_ceil * width].height;

                float q1 = h1 * (static_cast<float>(x_ceil) - x) + h2 * (x - static_cast<float>(x_floor));
                float q2 = h3 * (static_cast<float>(x_ceil) - x) + h4 * (x - static_cast<float>(x_floor));
                height = q1 * (static_cast<float>(y_ceil) - y) + q2 * (y - static_cast<float>(y_floor));

                bool p1 = points[x_floor + y_floor * width].placed;
                bool p2 = points[x_ceil + y_floor * width].placed;
                bool p3 = points[x_floor + y_ceil * width].placed;
                bool p4 = points[x_ceil + y_ceil * width].placed;

                placed = p1 || p2 || p3 || p4;
            }

            ConnectedPoint point = ConnectedPoint();
            point.height = height;
            point.placed = placed;
            newPoints[i + j * newWidth] = point;
        }
    }

    points = newPoints;
    width = newWidth;
    height = newHeight;
}

void DLATexture::Blur() {
    std::vector<ConnectedPoint> newPoints(width * height, ConnectedPoint());

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int staticWidth = static_cast<int>(width);

            int position = x + y * staticWidth;
            int upPosition = x + (y - 1) * staticWidth;
            int downPosition = x + (y + 1) * staticWidth;
            int leftPosition = (x - 1) + y * staticWidth;
            int rightPosition = (x + 1) + y * staticWidth;

            float upSample = 0.0f;
            float downSample = 0.0f;
            float leftSample = 0.0f;
            float rightSample = 0.0f;
            float sample = points[position].height * CENTER_SAMPLE;

            bool placed = points[position].placed;
            bool leftPlaced = false;
            bool rightPlaced = false;
            bool upPlaced = false;
            bool downPlaced = false;

            if (y != 0) {
                upPlaced = points[upPosition].placed;
                upSample = points[upPosition].height * UP_SAMPLE;
            }

            if (y != height-1) {
                downPlaced = points[downPosition].placed;
                downSample = points[downPosition].height * DOWN_SAMPLE;
            }

            if (x != 0) {
                leftPlaced = points[leftPosition].placed;
                leftSample = points[leftPosition].height * LEFT_SAMPLE;
            }

            if (x != width-1) {
                rightPlaced = points[rightPosition].placed;
                rightSample = points[rightPosition].height * RIGHT_SAMPLE;
            }

            newPoints[position].placed = placed || leftPlaced || rightPlaced || upPlaced || downPlaced;
            newPoints[position].height = sample + upSample + downSample + leftSample + rightSample;
        }
    }

    points = newPoints;
}


