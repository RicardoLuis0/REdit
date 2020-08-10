#pragma once

#include <stdint.h>

#include "IOLayer.h"

namespace MenuEngine {
    void init();
    int16_t getHeight();
    void draw(bool focused);
    bool handle_input(IOLayer::keypress key);
    bool is_fullscreen();
};
