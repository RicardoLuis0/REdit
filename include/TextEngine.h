#pragma once

#include "IOLayer.h"
#include "Text.h"

#include <cstdint>

namespace TextEngine {
    void init(int16_t menu_size,Text * t_data);
    void handle_input(IO::keypress);
    int save(const char * filename);
    void set_offset(int16_t menu_size);
    void redraw_full();
    void update_cursor();
};
