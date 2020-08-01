#pragma once

#include <cstdint>

namespace IO {
    void init();
    enum key_type {
        ARROW_LEFT,
        ARROW_UP,
        ARROW_RIGHT,
        ARROW_DOWN,
        KEY,
    };
    struct keypress {
        char key;
        uint8_t type;
        uint16_t count;
    };
    keypress get_key();
    void moveCursor(int16_t x,int16_t y);
    void writeChar(char c);
};
