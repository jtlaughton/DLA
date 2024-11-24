//
// Created by jtlau on 11/21/2024.
//

#include "../headers/dlatexture.h"

DLATexture::DLATexture() {
    sharpPoints = std::vector<ConnectedPoint>(width * height, ConnectedPoint());

    const unsigned int midX = width / 2;
    const unsigned int midY = height / 2;

    int position = midX + midY * width;
    sharpPoints[midX + midY * width].Place(-10, currentPlaced, midX, midY, position, sharpPoints);
    currentPlaced++;

    std::random_device rd;
    gen = std::mt19937(rd());
    distribDir = std::uniform_int_distribution<unsigned>(0, 3);
    distribX = std::uniform_int_distribution<unsigned>(0, width - 1);
    distribY = std::uniform_int_distribution<unsigned>(0, height - 1);
}

void DLATexture::Reset() {
    for (int i = 0; i < width * height; i++) {
        blurredPoints[i] = ConnectedPoint();
    }

    const unsigned int midX = width / 2;
    const unsigned int midY = height / 2;

    int position = midX + midY * width;
    sharpPoints[midX + midY * width].Place(-10, currentPlaced, midX, midY, position, sharpPoints);
    currentPlaced++;

    currentIteration = 0;
}

std::string DLATexture::GetBlurredByteStream() const {
    return GetByteStream(blurredPoints);
}

std::string DLATexture::GetSharpByteStream() const {
    return GetByteStream(sharpPoints);
}

std::string DLATexture::GetByteStream(const std::vector<ConnectedPoint>& vector) const {
    std::string byteString;

    std::cout << maxHeight << std::endl;

    for (const auto& point: vector) {
        const char floored = static_cast<char>(floorf(point.weightedHeight * 255.0f));

        byteString += floored;
        byteString += floored;
        byteString += floored;
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

    if (x != 0 && sharpPoints[left].placed)
        return left;

    if (x != width - 1 && sharpPoints[right].placed)
        return right;

    if (y != 0 && sharpPoints[up].placed)
        return up;

    if (y != height - 1 && sharpPoints[down].placed)
        return down;

    return -1;
}

bool DLATexture::RunIteration() {
    if (currentIteration == maxIterations)
        return false;

    currentIteration++;

    unsigned int randx = distribX(gen);
    unsigned int randy = distribY(gen);

    ConnectedPoint current = sharpPoints[randx + randy * width];

    while (current.placed) {
        current = sharpPoints[_randomMove(randx, randy)];
    }

    int currentAssigned = _nearbyAssigned(randx, randy);

    while (currentAssigned == -1) {
        _randomMove(randx, randy);
        currentAssigned = _nearbyAssigned(randx, randy);
    }

    int position = randx + randy * width;
    sharpPoints[position].Place(currentAssigned, currentPlaced, randx, randy, position, sharpPoints);
    currentPlaced++;
    sharpPoints[position].TryIncreaseHeight(sharpPoints, maxHeight);

    return true;
}

void DLATexture::BiLinearInterpolationBy2() {
    const int oldWidth = static_cast<int>(blurredWidth);
    const int oldHeight = static_cast<int>(blurredHeight);
    const int newWidth = oldWidth * 2;
    const int newHeight = oldHeight * 2;

    std::vector<ConnectedPoint> newPoints(newWidth * newHeight, ConnectedPoint());

    const float w_scale = static_cast<float>(oldWidth) / static_cast<float>(newWidth);
    const float h_scale = static_cast<float>(oldHeight) / static_cast<float>(newHeight);

    for (int i = 0; i < newWidth; i++) {
        for (int j = 0; j < newHeight; j++) {
            const float x = static_cast<float>(i) * w_scale;
            const float y = static_cast<float>(j) * h_scale;

            const int x_floor = static_cast<int>(floorf(x));
            const int x_ceil = std::min(oldWidth - 1, static_cast<int>(ceilf(x)));
            const int y_floor = static_cast<int>(floorf(y));
            const int y_ceil = std::min(oldHeight - 1, static_cast<int>(ceilf(y)));

            float tempHeight = 0.0f;
            bool placed = false;

            if (x_floor == x_ceil && y_floor == y_ceil) {
                tempHeight = blurredPoints[x_floor + y_floor * oldWidth].weightedHeight;
                placed = blurredPoints[x_floor + y_floor * oldWidth].placed;
            }
            else if (x_ceil == x_floor) {
                const float h1 = blurredPoints[x_floor + y_floor * oldWidth].weightedHeight;
                const float h2 = blurredPoints[x_floor + y_ceil * oldWidth].weightedHeight;

                tempHeight = h1 * (static_cast<float>(y_ceil) - y) + h2 * (y - static_cast<float>(y_floor));

                const bool p1 = blurredPoints[x_floor + y_floor * oldWidth].placed;
                const bool p2 = blurredPoints[x_floor + y_ceil * oldWidth].placed;

                placed = p1 || p2;
            }
            else if (y_ceil == y_floor) {
                const float h1 = blurredPoints[x_floor + y_floor * oldWidth].weightedHeight;
                const float h2 = blurredPoints[x_ceil + y_floor * oldWidth].weightedHeight;

                tempHeight = h1 * (static_cast<float>(x_ceil) - x) + h2 * (x - static_cast<float>(x_floor));

                const bool p1 = blurredPoints[x_floor + y_floor * oldWidth].placed;
                const bool p2 = blurredPoints[x_ceil + y_floor * oldWidth].placed;

                placed = p1 || p2;
            }
            else {
                const float h1 = blurredPoints[x_floor + y_floor * oldWidth].weightedHeight;
                const float h2 = blurredPoints[x_ceil + y_floor * oldWidth].weightedHeight;
                const float h3 = blurredPoints[x_floor + y_ceil * oldWidth].weightedHeight;
                const float h4 = blurredPoints[x_ceil + y_ceil * oldWidth].weightedHeight;

                const float q1 = h1 * (static_cast<float>(x_ceil) - x) + h2 * (x - static_cast<float>(x_floor));
                const float q2 = h3 * (static_cast<float>(x_ceil) - x) + h4 * (x - static_cast<float>(x_floor));
                tempHeight = q1 * (static_cast<float>(y_ceil) - y) + q2 * (y - static_cast<float>(y_floor));

                const bool p1 = blurredPoints[x_floor + y_floor * oldWidth].placed;
                const bool p2 = blurredPoints[x_ceil + y_floor * oldWidth].placed;
                const bool p3 = blurredPoints[x_floor + y_ceil * oldWidth].placed;
                const bool p4 = blurredPoints[x_ceil + y_ceil * oldWidth].placed;

                placed = p1 || p2 || p3 || p4;
            }

            ConnectedPoint point = ConnectedPoint();
            point.weightedHeight = tempHeight;
            point.placed = placed;
            newPoints[i + j * newWidth] = point;
        }
    }

    blurredWidth = newWidth;
    blurredHeight = newHeight;
    blurredPoints = newPoints;
}

void DLATexture::Blur() {
    std::vector<ConnectedPoint> newPoints(blurredWidth * blurredHeight, ConnectedPoint());

    for (int x = 0; x < blurredWidth; x++) {
        for (int y = 0; y < blurredHeight; y++) {
            const int staticWidth = static_cast<int>(blurredWidth);

            const int position = x + y * staticWidth;
            const int upPosition = x + (y - 1) * staticWidth;
            const int downPosition = x + (y + 1) * staticWidth;
            const int leftPosition = (x - 1) + y * staticWidth;
            const int rightPosition = (x + 1) + y * staticWidth;

            float upSample = 0.0f;
            float downSample = 0.0f;
            float leftSample = 0.0f;
            float rightSample = 0.0f;
            const float sample = blurredPoints[position].weightedHeight * CENTER_SAMPLE;

            const bool placed = blurredPoints[position].placed;
            bool leftPlaced = false;
            bool rightPlaced = false;
            bool upPlaced = false;
            bool downPlaced = false;

            if (y != 0) {
                upPlaced = blurredPoints[upPosition].placed;
                upSample = blurredPoints[upPosition].weightedHeight * UP_SAMPLE;
            }

            if (y != blurredHeight-1) {
                downPlaced = blurredPoints[downPosition].placed;
                downSample = blurredPoints[downPosition].weightedHeight * DOWN_SAMPLE;
            }

            if (x != 0) {
                leftPlaced = blurredPoints[leftPosition].placed;
                leftSample = blurredPoints[leftPosition].weightedHeight * LEFT_SAMPLE;
            }

            if (x != blurredWidth-1) {
                rightPlaced = blurredPoints[rightPosition].placed;
                rightSample = blurredPoints[rightPosition].weightedHeight * RIGHT_SAMPLE;
            }

            newPoints[position].placed = placed || leftPlaced || rightPlaced || upPlaced || downPlaced;
            newPoints[position].weightedHeight = sample + upSample + downSample + leftSample + rightSample;
        }
    }

    blurredPoints = newPoints;
}

void DLATexture::SharpUpscale() {
    int ratio = 2;

    std::vector<ConnectedPoint> startingPoints;
    std::vector<ConnectedPoint> pointsToPlace;
    const int newWidth = static_cast<int>(width) * ratio;
    const int newHeight = static_cast<int>(height) * ratio;
    std::vector<ConnectedPoint> newPoints(newWidth * newHeight, ConnectedPoint());

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int position = x + y * static_cast<int>(width);

            if (sharpPoints[position].placed) {
                pointsToPlace.push_back(sharpPoints[position]);

                if (sharpPoints[position].startingHeight) {
                    startingPoints.push_back(sharpPoints[position]);
                }
            }
        }
    }


    for (const auto& point : pointsToPlace) {
        int newXCurrent = ratio * point.x;
        int newYCurrent = ratio * point.y;

        if (point.indexConnection < 0) {
            newPoints[newXCurrent + newYCurrent * newWidth].Place(point.indexConnection, currentPlaced, newXCurrent, newYCurrent, newXCurrent + newYCurrent * newWidth, newPoints);
            currentPlaced++;
            continue;
        }

        ConnectedPoint next = sharpPoints[point.indexConnection];\

        const int newXNext = ratio * next.x;
        const int newYNext = ratio * next.y;

        int incrementX = 0;
        int incrementY = 0;

        if (newYCurrent == newYNext) {
            incrementX = newXCurrent > newXNext ? -1 : 1;
        }
        else if (newXCurrent == newXNext) {
            incrementY = newYCurrent > newYNext ? -1 : 1;
        }

        //float previous = -1;

        while ((newXCurrent != newXNext) || (newYCurrent != newYNext)) {
            const int position = newXCurrent + newYCurrent * newWidth;

            const int nextPosition = (newXCurrent + incrementX) + (newYCurrent + incrementY) * newWidth;

            newPoints[position].Place(nextPosition, currentPlaced, newXCurrent, newYCurrent, position, newPoints);
            currentPlaced++;

            newXCurrent += incrementX;
            newYCurrent += incrementY;

            // if (previous == -1) {
            //     float maxHeight = 1.0f;
            //     for (const int i : newPoints[position].downStream) {
            //         maxHeight = std::max(maxHeight, newPoints[i].height);
            //     }
            //
            //     newPoints[position].height = maxHeight;
            //     previous = newPoints[position].height;
            // }
            // else {
            //     newPoints[position].height = previous + (1.0f - (1.0f / (1.0f + previous)));
            //     previous = newPoints[position].height;
            // }
        }

        int nextPosition = newXNext + newYNext * newWidth;
        // if (newPoints[nextPosition].placed) {
        //     newPoints[nextPosition].TryIncreaseHeight(newPoints, maxHeight);
        // }
        // else {
        //     newPoints[nextPosition].Place(-1, currentPlaced, newXNext, newYNext, nextPosition, newPoints);
        //     newPoints[nextPosition].height = previous + (1.0f - (1.0f / (1.0f + previous)));
        //     currentPlaced++;
        // }

        if (!newPoints[nextPosition].placed) {
            newPoints[nextPosition].Place(-1, currentPlaced, newXNext, newYNext, nextPosition, newPoints);
            currentPlaced++;
        }
    }

    for (const auto& point : startingPoints) {
        int newXCurrent = ratio * point.x;
        int newYCurrent = ratio * point.y;
        int position = newXCurrent + newYCurrent * newWidth;
        newPoints[position].TryIncreaseHeight(newPoints, maxHeight);
    }

    sharpPoints = newPoints;
    width = newWidth;
    height = newHeight;
}

void DLATexture::CombineBlurredAndSharp() {
    std::vector<ConnectedPoint> newBlurred(width * height, ConnectedPoint());

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int position = x + y * static_cast<int>(width);

            if (sharpPoints[position].placed && blurredPoints[position].weightedHeight < 0.2f) {
                newBlurred[position].weightedHeight = .75f * sharpPoints[position].weightedHeight + .25f * blurredPoints[position].weightedHeight;
            }
            else if (sharpPoints[position].placed && blurredPoints[position].weightedHeight > 0.7f) {
                newBlurred[position].weightedHeight = .1f * sharpPoints[position].weightedHeight + .9f * blurredPoints[position].weightedHeight;
            }
            else if (sharpPoints[position].placed) {
                newBlurred[position].weightedHeight = .5f * sharpPoints[position].weightedHeight + .5f * blurredPoints[position].weightedHeight;
            }
            else {
                newBlurred[position].weightedHeight = blurredPoints[position].weightedHeight;
            }

            newBlurred[position].placed = sharpPoints[position].placed || blurredPoints[position].placed;
        }
    }

    blurredPoints = newBlurred;
}

void DLATexture::RunSequence() {
    RunAllIterations();

    blurredPoints = std::vector<ConnectedPoint>(sharpPoints);

    for (int i = 0; i < 4; i++) {
        SharpUpscale();

        distribX = std::uniform_int_distribution<unsigned>(0, width - 1);
        distribY = std::uniform_int_distribution<unsigned>(0, height - 1);

        currentIteration = 0;
        maxIterations *= 4;

        RunAllIterations();

        BiLinearInterpolationBy2();
        Blur();

        CombineBlurredAndSharp();
    }

    BiLinearInterpolationBy2();

    for (int x = 0; x < 5; x++) {
        Blur();
    }
}

void DLATexture::RunAllIterations() {
    while (currentIteration != maxIterations) {
        RunIteration();
    }
}

