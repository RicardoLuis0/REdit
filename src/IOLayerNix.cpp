#include "IOLayer.h"
#include <stdarg.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>

namespace TEdit {
    int tedit_main(int,char **);
}

int main(int argc,char ** argv) {
    return TEdit::tedit_main(argc,argv);
}

namespace TEdit::IOLayer {
    termios old_mode;
    
    static void cleanup(){
        tcsetattr(0,TCSANOW,&old_mode);
        moveCursor(0,25);
    }
    
    void init(){
        tcgetattr(0,&old_mode);
        termios new_mode=old_mode;
        new_mode.c_lflag&=~(ECHO|ICANON);
        tcsetattr(0,TCSANOW,&new_mode);
        atexit(cleanup);
        /*
        while(true){
            moveCursor(0,0);
            printf("\n%d",get_key().key);
        }
        */
        //while(true)printf("\n%d",getchar());
    }
    
    keypress get_key(){
    retry:
        int kc=getchar();
        switch(kc){
        case '\e':
            switch(getchar()){
            case 91:
                switch(getchar()){
                case 'A':
                    return {0,ARROW_UP,1};
                case 'B':
                    return {0,ARROW_DOWN,1};
                case 'C':
                    return {0,ARROW_RIGHT,1};
                case 'D':
                    return {0,ARROW_LEFT,1};
                }
            case '\e':
                return {(char)'\e',KEY,1};
            default:
                goto retry;
            }
        case 127:
            return {(char)'\b',KEY,1};
        default:
            return {(char)kc,KEY,1};
        }
    }
    
    void moveCursor(int16_t x,int16_t y){
        printf("\e[%d;%dH",(uint32_t)y+1,(uint32_t)x+1);
    }
    
    void writeChar(char c){
        printf("%c",c);
    }
    
    void writeChars(const char * s,size_t n){
        printf("%.*s",(int)n,s);
    }
    
    void writeStr(const char * s){
        printf("%s",s);
    }
    
    int term_colors[]={
        30,//BLACK,0
        34,//BLUE,1
        32,//GREEN,2
        36,//CYAN,3
        31,//RED,4
        35,//MAGENTA,5
        33,//BROWN,6
        37,//LIGHT_GREY,7
        90,//DARK_GREY,8
        94,//LIGHT_BLUE,9
        92,//LIGHT_GREEN,10
        96,//LIGHT_CYAN,11
        91,//LIGHT_RED,12
        95,//LIGHT_MAGENTA,13
        93,//LIGHT_BROWN,14
        97,//WHITE,15
    };
    
    color fgcolor=WHITE;
    color bgcolor=BLACK;
    
    void setColor(color fg,color bg){
        fgcolor=fg;
        bgcolor=bg;
        printf("\e[%d;%dm",term_colors[fg],term_colors[bg]+10);
    }
    
    void fillLine(int16_t line,int16_t rows,char c,color fg,color bg){
        setColor(fg,bg);
        for(uint32_t i=0;i<rows;i++){
            moveCursor(0,i+line);
            for(uint32_t j=0;j<80;j++){
                writeChar(c);
            }
        }
        setColor(fgcolor,bgcolor);
    }
    
    [[noreturn]] void exit_error(const char * fmt,...){
        va_list args;
        va_start(args,fmt);
        vfprintf(stderr,fmt,args);
        va_end(args);
        exit(1);
    }
}
