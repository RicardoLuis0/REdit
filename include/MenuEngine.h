#pragma once

#include <cstdint>

#include "IOLayer.h"

namespace MenuEngine {
    void init();
    int16_t getHeight();
    void draw(bool focused);
    bool handle_input(IO::keypress key);
    bool is_fullscreen();
};
