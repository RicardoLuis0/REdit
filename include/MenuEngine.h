#pragma once

#include <cstdint>

#include "IOLayer.h"

namespace MenuEngine {
    void init();
    int16_t getHeight();
    void draw(bool focused);
    void handle_input(IO::keypress key);
};
