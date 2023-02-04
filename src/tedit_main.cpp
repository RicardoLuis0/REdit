#include "IOLayer.h"
#include "TextEngine.h"
#include "MenuEngine.h"
#include "Util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace TEdit {
    
    bool do_loop;
    
    int tedit_main(int argc,char ** argv) {
        do_loop=true;
        IOLayer::init();
        MenuEngine::init();
        IOLayer::fillLine(0,25,' ',IOLayer::WHITE,IOLayer::BLACK);
        if(argc==2){
            if(strlen(argv[1])>40){
                IOLayer::writeStr("\nFilename Too long\n");
                return 1;
            }else{
                TextEngine::initFile(MenuEngine::getHeight(),argv[1]);
            }
        }else if(argc>2){
            IOLayer::writeStr("\nInvalid Number of Arguments, must be zero or 1\n");
            return 1;
        }else{
            TextEngine::init(MenuEngine::getHeight(),new(malloc(sizeof(Text))) Text());
        }
        TextEngine::redraw_full();
        MenuEngine::draw(false);
        bool in_menu=false;
        while(do_loop){
            auto key=IOLayer::get_key();
            if(in_menu){
                in_menu=MenuEngine::handle_input(key);
                if(!in_menu&&MenuEngine::is_fullscreen()){
                    TextEngine::redraw_full();
                }
            }else if(key.type==IOLayer::ALT||(key.type==IOLayer::KEY&&(key.key=='\e'||key.key==20))){
                in_menu=true;
            }else{
                TextEngine::handle_input(key);
            }
            if(in_menu||!MenuEngine::is_fullscreen()){
                MenuEngine::draw(in_menu);
            }
        }
        MenuEngine::quit();
        IOLayer::fillLine(0,25,' ',IOLayer::WHITE,IOLayer::BLACK);
        IOLayer::moveCursor(0,0);
        IOLayer::setColor(IOLayer::WHITE,IOLayer::BLACK);
        return 0;
    }
    
}
