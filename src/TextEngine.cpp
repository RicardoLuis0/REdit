#include "TextEngine.h"

namespace TextEngine {
    
    static int isgraph(int c){
        return (c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||(c>='!'&&c<='/')||(c>=':'&&c<='@')||(c>='['&&c<='`')||(c>='{'&&c<='~');
    }
    
    int16_t x,y;
    
    int16_t x_min,x_max,y_min,y_max;
    
    int16_t x_mmin,x_mmax,y_mmin,y_mmax;
    
    uint32_t view_x,view_y;
    
    static void x_plus(bool wrap=false){
        if(x==x_max){
            if(y==y_max){
                if(wrap){
                    x=x_min;
                    y=y_min;
                }
            }else{
                x=x_min;
                y++;
            }
        }else{
            x++;
        }
        IO::moveCursor(x,y);
    }
    
    static void x_minus(bool wrap=false,bool use_xmin=true){
        if(x==x_min){
            if(y==y_min){
                if(wrap){
                    x=use_xmin?x_min:x_max;
                    y=y_max;
                }
            }else{
                x=use_xmin?x_min:x_max;
                y--;
            }
        }else{
            x--;
        }
        IO::moveCursor(x,y);
    }
    
    static void y_plus(bool wrap=false){
        if(y==y_max){
            if(wrap){
                x=x_min;
                y=y_min;
            }
        }else{
            x=x_min;
            y++;
        }
        IO::moveCursor(x,y);
    }
    
    static void y_minus(bool wrap=false){
        if(y==y_min){
            if(wrap){
                x=x_max;
                y=y_max;
            }
        }else{
            x=x_max;
            y--;
        }
        IO::moveCursor(x,y);
    }
    
    void init(int16_t off){
        x_min=0;
        x_mmin=-1;
        x=0;
        x_max=79;
        x_mmax=80;
        y_min=0;
        y_mmin=-1;
        y=0;
        y_max=24-off;
        y_mmax=25-off;
        IO::moveCursor(x,y);
    }
    
    void handle_input(IO::keypress key){
        switch(key.type){
            case IO::ARROW_UP:
                if(y>y_min)y--;
                IO::moveCursor(x,y);
                break;
            case IO::ARROW_DOWN:
                if(y<y_max)y++;
                IO::moveCursor(x,y);
                break;
            case IO::ARROW_LEFT:
                if(x>x_min)x--;
                IO::moveCursor(x,y);
                break;
            case IO::ARROW_RIGHT:
                if(x<x_max)x++;
                IO::moveCursor(x,y);
                break;
            case IO::KEY:
                while(key.count--){
                    switch(key.key){
                    case ' ':
                        IO::writeChar(' ');
                        x_plus();
                        break;
                    case '\b':
                        x_minus();
                        IO::writeChar(' ');
                        IO::moveCursor(x,y);
                        break;
                    case '\n':
                    case '\r':
                        y_plus();
                        break;
                    case '\t':
                        do{
                            IO::writeChar(' ');
                            x_plus();
                        }while(x%4);
                    default:
                        if(isgraph(key.key)){
                            IO::writeChar(key.key);
                            x_plus();
                        }
                    }
                }
                break;
        }
    }
}
