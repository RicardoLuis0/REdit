#pragma once

#include "IOLayer.h"
#include "Text.h"

#include <stdint.h>

namespace TEdit::TextEngine {
    void initFile(int16_t menu_size,const char * filename);
    void init(int16_t menu_size,Text * t_data);
    
    void reInit(Text * t_data);
    void reInitFile(const char * filename);
    
    void handle_input(IOLayer::keypress);
    int save(const char * filename);
    void set_offset(int16_t menu_size);
    void redraw_full();
    void update_cursor();
};
