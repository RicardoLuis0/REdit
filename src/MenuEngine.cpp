#include "MenuEngine.h"
#include "TextEngine.h"
#include "IOLayer.h"
#include "Util.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX(x,y)((x)>(y)?(x):(y))
#define MIN(x,y)((x)<(y)?(x):(y))

namespace TEdit {
    extern bool do_loop;
}

namespace TEdit::MenuEngine {
    
    int16_t line=24;
    int16_t height=1;
    
    char * filename=nullptr;
    size_t filename_len=0;
    constexpr size_t filename_max=40;
    
    void setfile(const char * s){
        strncpy(filename,s,filename_max);
        filename[filename_max]='\0';
        filename_len=strlen(filename);
    }
    
    bool fullscreen=false;
    
    enum{
        STATE_NONE,
        STATE_SAVING,
        STATE_QUITCONFIRM,
        STATE_NEWCONFIRM,
        STATE_OPENCONFIRM,
        STATE_OPENING,
    } state=STATE_NONE;
    
    void init(){
        filename=(char*)calloc(filename_max+1,1);
    }
    
    void quit(){
        free(filename);
    }
    
    int16_t getHeight(){
        return height;
    }
    
    void draw(bool focused){
        if(focused){
            IOLayer::fillLine(line,height,' ',IOLayer::LIGHT_CYAN,IOLayer::CYAN);
            IOLayer::setColor(IOLayer::LIGHT_CYAN,IOLayer::CYAN);
            IOLayer::moveCursor(0,line);
            switch(state){
            case STATE_NONE:
                IOLayer::writeStr("S - Save, Q - Quit, N - New, O - Open, F - Fullscreen");
                break;
            case STATE_SAVING:
                IOLayer::writeStr("Save As: ");
                IOLayer::writeChars(filename,filename_len);
                return;
            case STATE_QUITCONFIRM:
                IOLayer::writeStr("Unsaved changes will be lost, Quit? (Y/N)");
                return;
            case STATE_NEWCONFIRM:
                IOLayer::writeStr("Unsaved changes will be lost, create New file? (Y/N)");
                return;
            case STATE_OPENCONFIRM:
                IOLayer::writeStr("Unsaved changes will be lost, Open another file? (Y/N)");
                return;
            case STATE_OPENING:
                IOLayer::writeStr("Open File: ");
                IOLayer::writeChars(filename,filename_len);
                return;
            }
        }else{
            IOLayer::fillLine(line,height,' ',IOLayer::BLACK,IOLayer::LIGHT_GREY);
            IOLayer::setColor(IOLayer::BLACK,IOLayer::LIGHT_GREY);
            IOLayer::moveCursor(0,line);
            IOLayer::writeStr("File: ");
            if(filename_len>0){
                IOLayer::writeChars(filename,filename_len);
            }else{
                IOLayer::writeStr("...");
            }
        }
        IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
        TextEngine::update_cursor();
    }
    
    bool handle_input(IOLayer::keypress key){
        switch(state){
        case STATE_SAVING:
        case STATE_OPENING:
            if(key.key=='\n'||key.key=='\r'){//confirm
                if(state == STATE_SAVING){
                    if(TextEngine::save(filename)){
                        state=STATE_NONE;
                        IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
                        TextEngine::update_cursor();
                        return false;
                    }
                }else if(state == STATE_OPENING){
                    state=STATE_NONE;
                    IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
                    TextEngine::reInitFile(filename);
                    return false;
                }
                return true;
            }else if(key.key=='\e'){//cancel
                state=STATE_NONE;
                IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
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
        case STATE_NEWCONFIRM:
        case STATE_OPENCONFIRM:
            if(key.key=='y'||key.key=='Y'||key.key=='q'||key.key=='Q'||key.key=='\n'||key.key=='\r'){
                switch(state){
                case STATE_QUITCONFIRM:
                    do_loop=false;
                    return false;
                case STATE_NEWCONFIRM:
                    state=STATE_NONE;
                    IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
                    TextEngine::reInit(new(malloc(sizeof(Text))) Text());
                    filename_len=0;
                    filename[0]='\0';
                    return false;
                case STATE_OPENCONFIRM:
                    state=STATE_OPENING;
                    return true;
                default:
                    __builtin_unreachable();
                }
            }else if(key.key=='n'||key.key=='N'||key.key=='\e'){
                state=STATE_NONE;
                IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
                TextEngine::update_cursor();
                return false;
            }
            return true;
        case STATE_NONE:
            if(key.type==IOLayer::ALT||(key.type==IOLayer::KEY&&(key.key=='\e'||key.key==20))){
                return false;
            }else if(key.key=='s'||key.key=='S'){
                state=STATE_SAVING;
            }else if(key.key=='q'||key.key=='Q'){
                state=STATE_QUITCONFIRM;
            }else if(key.key=='n'||key.key=='N'){
                state=STATE_NEWCONFIRM;
            }else if(key.key=='o'||key.key=='O'){
                state=STATE_OPENCONFIRM;
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
