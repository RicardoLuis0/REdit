#define WIN32_LEAN_AND_MEAN

#include "windows.h"

namespace IO {
    
    HANDLE hStdin;
    HANDLE hStdout;
    
    void init(){
        hStdin=GetStdHandle(STD_INPUT_HANDLE);
        hStdout=GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleScreenBufferSize(hStdout,{80,25});
    }
    
    keypress get_key(){
        INPUT_RECORD input;
        DWORD n;
        for(;;){
            ReadConsoleInput(hStdin,&input,1,&n);
            if(n>0&&input.EventType==KEY_EVENT&&input.Event.KeyEvent.bKeyDown){
                auto & k=input.Event.KeyEvent;
                if(k.uChar.AsciiChar!=0){
                    return {k.uChar.AsciiChar,KEY,k.wRepeatCount};
                }else{
                    switch(k.wVirtualKeyCode){
                    case VK_LEFT:
                        return {0,ARROW_LEFT,k.wRepeatCount};
                    case VK_UP:
                        return {0,ARROW_UP,k.wRepeatCount};
                    case VK_RIGHT:
                        return {0,ARROW_RIGHT,k.wRepeatCount};
                    case VK_DOWN:
                        return {0,ARROW_DOWN,k.wRepeatCount};
                    }
                }
            }
        }
    }
    
    void moveCursor(int16_t x,int16_t y){
        SetConsoleCursorPosition(hStdout,{x,y});
    }
    
    void writeChar(char c){
        WriteConsole(hStdout,&c,1,NULL,NULL);
    }
}
