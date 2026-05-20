#include "server/shared.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::printf("Usage: %s <port>\n", argv[0]);
    }

    return 0;
}
