#include "MenuEngine.h"
#include "IOLayer.h"

namespace MenuEngine {
    
    int16_t line;
    int16_t height;
    int16_t position;
    
    void init(){
        line=24;
        height=1;
        draw(false);
    }
    
    int16_t getHeight(){
        return height;
    }
    
    void draw(bool focused){
        IO::fillLine(line,height,' ',IO::BLACK,IO::DARK_GREY);
    }
    
    void handle_input(IO::keypress key){
        //TODO
    }
    
}
