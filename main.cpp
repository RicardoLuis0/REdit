#include "IOLayer.h"
#include <iostream>

int main() {
    IO::init();
    int16_t x=0,y=0;
    auto c_plus=[&x,&y](){
        x++;
        if(x==80){
            x=0;
            y++;
            if(y==25){
                y=0;
            }
        }
        IO::moveCursor(x,y);
    };
    auto c_minus=[&x,&y](){
        x--;
        if(x==-1){
            x=79;
            y--;
            if(y==-1){
                y=24;
            }
        }
        IO::moveCursor(x,y);
    };
    auto l_plus=[&x,&y](){
        x=0;
        y++;
        if(y==25){
            y=0;
        }
        IO::moveCursor(x,y);
    };
    while(1){
        IO::moveCursor(x,y);
        auto key=IO::get_key();
        switch(key.type){
            case IO::ARROW_UP:
                if(y>0)y--;
                break;
            case IO::ARROW_DOWN:
                if(y<24)y++;
                break;
            case IO::ARROW_LEFT:
                if(x>0)x--;
                break;
            case IO::ARROW_RIGHT:
                if(x<79)x++;
                break;
            case IO::KEY:
                while(key.count--){
                    switch(key.key){
                    case ' ':
                        c_plus();
                        break;
                    case '\b':
                        c_minus();
                        IO::writeChar(' ');
                        break;
                    case '\n':
                    case '\r':
                        l_plus();
                        break;
                    default:
                        if(isgraph(key.key)){
                            IO::writeChar(key.key);
                            c_plus();
                        }
                    }
                }
                break;
        }
    }
    return 0;
}
