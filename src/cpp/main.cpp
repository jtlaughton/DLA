#include <iostream>
#include "../headers/renderer.h"

int main() {
    auto renderer = Renderer();

    if (const int initResult = renderer.Init(); initResult != 0) {
        return -1;
    }

    while (renderer.Running()) {
        renderer.Update();
    };

    renderer.End();

    return 0;
}
