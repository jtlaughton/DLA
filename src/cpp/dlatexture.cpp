//
// Created by jtlau on 11/21/2024.
//

#include "../headers/dlatexture.h"

DLATexture::DLATexture() {
    points = std::vector<bool>(WIDTH * HEIGHT, false);

    const unsigned int midX = WIDTH / 2;
    const unsigned int midY = HEIGHT / 2;

    points[midX + midY * WIDTH] = true;

    std::random_device rd;
    gen = std::mt19937(rd());
    distribDir = std::uniform_int_distribution<unsigned>(0, 3);
    distribX = std::uniform_int_distribution<unsigned>(0, WIDTH - 1);
    distribY = std::uniform_int_distribution<unsigned>(0, HEIGHT - 1);
}

void DLATexture::Reset() {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        points[i] = false;
    }

    const unsigned int midX = WIDTH / 2;
    const unsigned int midY = HEIGHT / 2;

    points[midX + midY * WIDTH] = true;

    currentIteration = 0;
}

std::string DLATexture::GetByteStream() {
    std::string byteString;

    for (const auto point : points) {
        if (point) {
            // set to white
            byteString += 0xFF;
            byteString += 0xFF;
            byteString += 0xFF;
        }
        else {
            // set to very dark grey
            byteString += 0x11;
            byteString += 0x11;
            byteString += 0x11;
        }
    }

    return byteString;
}

unsigned int DLATexture::_randomMove(unsigned int& x, unsigned int& y) {
    switch (distribDir(gen)) {
        case 0:
            y += 1;
            if (y == HEIGHT) {
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
            if (x == WIDTH) {
                x -= 2;
            }
            break;
        default:
            break;
    }

    return x + y * WIDTH;
}

bool DLATexture::_nearbyAssigned(unsigned int x, unsigned int y) {
    return  (x != 0 && points[(x - 1) + y * WIDTH]) ||
            (x != WIDTH - 1 && points[(x + 1) + y * WIDTH]) ||
            (y != HEIGHT - 1 && points[x + (y + 1) * WIDTH]) ||
            (y != 0 && points[x + (y - 1) * WIDTH]);
}

bool DLATexture::RunIteration() {
    if (currentIteration == maxIterations)
        return false;

    currentIteration++;

    unsigned int randx = distribX(gen);
    unsigned int randy = distribY(gen);

    bool current = points[randx + randy * WIDTH];

    while (current) {
        current = points[_randomMove(randx, randy)];
    }

    if (_nearbyAssigned(randx, randy)) {
        points[randx + randy * WIDTH] = true;
        return true;
    }

    unsigned int currentPos = 0;
    while (!_nearbyAssigned(randx, randy)) {
        currentPos = _randomMove(randx, randy);
    }

    points[currentPos] = true;

    return true;
}
