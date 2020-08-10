#include "IOLayer.h"
#include "TextEngine.h"
#include "MenuEngine.h"

bool do_loop=true;

int tedit_main(int argc,char ** argv) {
    IOLayer::init();
    Text data;
    if(argc==2){
        
    }else if(argc>2){
        IOLayer::writeStr("\nInvalid Number of Arguments, must be zero or 1\n");
        return 1;
    }
    MenuEngine::init();
    TextEngine::init(MenuEngine::getHeight(),&data);
    bool in_menu=false;
    while(do_loop){
        auto key=IOLayer::get_key();
        if(in_menu){
            in_menu=MenuEngine::handle_input(key);
            if(!in_menu&&MenuEngine::is_fullscreen()){
                TextEngine::redraw_full();
            }
        }else if(key.type==IOLayer::ALT){
            in_menu=true;
        }else{
            TextEngine::handle_input(key);
        }
        if(in_menu||!MenuEngine::is_fullscreen()){
            MenuEngine::draw(in_menu);
        }
    }
    IOLayer::fillLine(0,25,' ',IOLayer::WHITE,IOLayer::BLACK);
    IOLayer::moveCursor(0,0);
    return 0;
}
