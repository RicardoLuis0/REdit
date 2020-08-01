#include "IOLayer.h"
#include "TextEngine.h"
#include "MenuEngine.h"

int main() {
    IO::init();
    MenuEngine::init();
    TextEngine::init(MenuEngine::getHeight());
    bool in_menu=false;
    while(1){
        auto key=IO::get_key();
        if(key.type==IO::ALT){
            in_menu=!in_menu;
        }else if(in_menu){
            MenuEngine::handle_input(key);
        }else{
            TextEngine::handle_input(key);
        }
        MenuEngine::draw(in_menu);
    }
    return 0;
}
