#include "MenuEngine.h"
#include "TextEngine.h"
#include "IOLayer.h"

#include <string.h>
#include <stdlib.h>

#define MAX(x,y)((x)>(y)?(x):(y))
#define MIN(x,y)((x)<(y)?(x):(y))

extern bool do_loop;

namespace TextEngine {
    int isgraph(int c);
}

using TextEngine::isgraph;

namespace MenuEngine {
    
    int16_t line=24;
    int16_t height=1;
    
    char * filename=nullptr;
    size_t filename_len=0;
    constexpr size_t filename_max=40;
    
    bool fullscreen=false;
    
    enum{
        STATE_NONE,
        STATE_SAVING,
        STATE_QUITCONFIRM,
    } state=STATE_NONE;
    
    void init(){
        filename=(char*)calloc(filename_max+1,1);
        draw(false);
    }
    
    int16_t getHeight(){
        return height;
    }
    
    void draw(bool focused){
        if(focused){
            IO::fillLine(line,height,' ',IO::BLACK,IO::WHITE);
            IO::setColor(IO::DARK_GREY,IO::WHITE);
            IO::moveCursor(0,line);
            switch(state){
            case STATE_NONE:
                IO::writeStr("S - Save, Q - Quit, F - Fullscreen");
                break;
            case STATE_SAVING:
                IO::writeStr("Save As: ");
                IO::writeChars(filename,filename_len);
                return;
            case STATE_QUITCONFIRM:
                IO::writeStr("Quit? (Y/N)");
                return;
            }
        }else{
            IO::fillLine(line,height,' ',IO::BLACK,IO::DARK_GREY);
            IO::setColor(IO::WHITE,IO::DARK_GREY);
            IO::moveCursor(0,line);
            IO::writeStr("File: ");
            if(filename_len>0){
                IO::writeChars(filename,filename_len);
            }else{
                IO::writeStr("...");
            }
        }
        IO::setColor(IO::WHITE,IO::BLACK);
        TextEngine::update_cursor();
    }
    
    bool handle_input(IO::keypress key){
        switch(state){
        case STATE_SAVING:
            if(key.key=='\n'||key.key=='\r'){//confirm
                if(TextEngine::save(filename)){
                    state=STATE_NONE;
                    IO::setColor(IO::WHITE,IO::BLACK);
                    TextEngine::update_cursor();
                    return false;
                }
                return true;
            }else if(key.key=='\e'){//cancel
                state=STATE_NONE;
                IO::setColor(IO::WHITE,IO::BLACK);
                TextEngine::update_cursor();
                return false;
            }else if(key.key=='\b'&&filename_len>0){
                filename_len--;
                filename[filename_len]='\0';
            }else if((isgraph(key.key)||key.key==' ')&&filename_len<filename_max){
                filename[filename_len]=key.key;
                filename_len++;
                filename[filename_len]='\0';
            }
            return true;
        case STATE_QUITCONFIRM:
            if(key.key=='y'||key.key=='Y'||key.key=='\n'||key.key=='\r'){
                do_loop=false;
                return false;
            }else if(key.key=='n'||key.key=='N'||key.key=='\e'){
                state=STATE_NONE;
                IO::setColor(IO::WHITE,IO::BLACK);
                TextEngine::update_cursor();
                return false;
            }
            return true;
        case STATE_NONE:
            if(key.type==IO::ALT){
                return false;
            }else if(key.key=='s'||key.key=='S'){
                state=STATE_SAVING;
            }else if(key.key=='q'||key.key=='Q'){
                state=STATE_QUITCONFIRM;
            }else if(key.key=='f'||key.key=='F'){
                fullscreen=!fullscreen;
                if(fullscreen){
                    TextEngine::set_offset(0);
                }else{
                    TextEngine::set_offset(height);
                }
                return false;
            }
            return true;
        default:
            return true;
        }
    }
    
    bool is_fullscreen(){
        return fullscreen;
    }
    
}
