//
// Created by jtlau on 11/21/2024.
//

#include "../headers/dlatexture.h"

DLATexture::DLATexture(){
    points = std::vector<bool>(WIDTH * HEIGHT, false);

    const unsigned int x = WIDTH / 2;
    const unsigned int y = HEIGHT / 2;

    points[x + y * WIDTH] = true;
}

std::string DLATexture::GetByteStream() {
    std::string byteString = "";

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

void DLATexture::RunIteration() {
    // do nothing for now
}
