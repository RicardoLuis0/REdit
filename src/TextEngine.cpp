#include "TextEngine.h"
#include "Text.h"

#include <cstdio>

namespace TextEngine {
    
    static int isgraph(int c){
        return (c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||(c>='!'&&c<='/')||(c>=':'&&c<='@')||(c>='['&&c<='`')||(c>='{'&&c<='~');
    }
    
    int16_t x,y;
    
    int16_t x_min,x_max,y_min,y_max;
    
    int16_t x_mmin,x_mmax,y_mmin,y_mmax;
    
    uint32_t view_x,view_y;
    
    Text data;
    
    static void redraw_line(size_t i){
        IO::moveCursor(0,i);
        IO::fillLine(i,1,' ',IO::WHITE,IO::BLACK);
        size_t max=data.size();
        if((i+view_y)>max)return;
        auto &l=data.get(i+view_y);
        if(view_x<l.len()){
            IO::writeChars(l.get()+view_x,l.len()-view_x);
        }
        IO::moveCursor(x,y);
    }
    
    static void redraw_full(){//FULL REDRAW, SLOW
        IO::fillLine(0,24,' ',IO::WHITE,IO::BLACK);
        size_t max=data.size();
        for(int i=0;i<24;i++){
            if((i+view_y)>=max)break;
            auto &l=data.get(i+view_y);
            if(view_x<l.len()){
                IO::moveCursor(0,i);
                IO::writeChars(l.get()+view_x,l.len()-view_x);
            }
        }
        IO::moveCursor(x,y);
    }
    
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
    
    static void y_minus(bool wrap=false,bool use_xmin=true){
        if(y==y_min){
            if(wrap){
                x=use_xmin?x_min:x_max;
                y=y_max;
            }
        }else{
            x=use_xmin?x_min:x_max;
            y--;
        }
        IO::moveCursor(x,y);
    }
    
    static void write(char c){
        auto &l=data.get(y+view_y);
        if((x+view_x)<(l.len())){
            l.insert(c,x+view_x);
            x_plus();
            redraw_line(y);
        }else{
            l.insert(c,x+view_x);
            IO::writeChar(c);
            x_plus();
        }
    }
    
    static void erase(){
        if(x==0){
            if(y>0){
                if(data.size()>(view_y+y)){
                    auto & a=data.get(view_y+y-1);
                    size_t pos=a.len();
                    auto & b=data.get(view_y+y);
                    a.append(b);
                    data.erase(view_y+y);
                    y_minus();
                    while(!((pos>=view_x)&&pos<(view_x+x_max))){
                        if(pos>view_x){
                            view_x+=x_max;
                        }else{
                            view_x-=x_max;
                        }
                    }
                    x=pos-view_x;
                    redraw_full();
                }else{
                    size_t pos=data.get(view_y+y-1).len();
                    y_minus();
                    while(!(pos>view_x)&&pos<(view_x+x_max)){
                        if(pos>view_x){
                            view_x+=x_max;
                        }else{
                            view_x-=x_max;
                        }
                    }
                    x=pos-view_x;
                }
            }
        }else{
            x_minus();
            auto & l=data.get(view_y+y);
            l.erase(view_x+x);
            redraw_line(y);
        }
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
        view_x=0;
        view_y=0;
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
                if(x>x_min){
                    x--;
                }else if(y>y_min){
                    y--;
                    size_t pos=data.get(view_y+y).len();
                    while(!(pos>view_x)&&pos<(view_x+x_max)){
                        if(pos>view_x){
                            view_x+=x_max;
                        }else{
                            view_x-=x_max;
                        }
                    }
                    x=pos-view_x;
                }
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
                        write(' ');
                        break;
                    case '\b':
                        erase();
                        break;
                    case '\n':
                    case '\r':
                        if((y+view_y)<data.size()){
                            data.insert(y+1,data.get(y+view_y).split(x+view_x));
                        }
                        y_plus();
                        redraw_full();
                        break;
                    case '\t':
                        do{
                            write(' ');
                        }while(x%4);
                        break;
                    default:
                        if(isgraph(key.key)){
                            write(key.key);
                        }
                    }
                }
                break;
        }
    }
    
    void save(const char * filename){
        FILE * f=fopen(filename,"w");
        for(size_t i=0;i<data.size();i++){
            fputs(data.get(i).get(),f);
            fputc('\n',f);
        }
        fclose(f);
    }
    
}
