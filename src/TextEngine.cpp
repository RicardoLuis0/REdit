#include "TextEngine.h"
#include "MenuEngine.h"
#include "Util.h"
#include <ctype.h>
#include <stdlib.h>

#include <stdio.h>

namespace TEdit::TextEngine {
    
    uint32_t x, y;
    
    uint32_t lpos;
    
    uint32_t x_min,
             x_max,
             y_min,
             y_max;
    
    uint32_t x_mmax,
             y_mmax;
    
    uint32_t view_x;
    
    uint32_t view_y;
    
    bool virtual_whitespace=true;
    
    Text * data;
    
    static void redraw_line(int i)
    {
        const size_t ypos = view_y + i;
        
        IOLayer::moveCursor(0, i);
        IOLayer::fillLine(i, 1, ' ', IOLayer::WHITE, IOLayer::BLACK);
        IOLayer::setColor(IOLayer::WHITE, IOLayer::BLACK);
        
        size_t n = data->size();
        
        //don't draw virtual lines
        if(ypos >= n) return;
        
        auto &l = data->get(ypos);
        if(view_x < l.len())
        {
            size_t n = l.len() - view_x;
            IOLayer::writeChars(l.get() + view_x, n >= x_mmax ? x_mmax : n);
        }
        IOLayer::moveCursor(x, y);
    }
    
    void redraw_full()
    {   //FULL REDRAW, SLOW
        IOLayer::fillLine(0, y_mmax, ' ', IOLayer::WHITE, IOLayer::BLACK);
        IOLayer::setColor(IOLayer::WHITE, IOLayer::BLACK);
        size_t n = data->size();
        for(uint i = 0; i < y_mmax; i++)
        {
            const size_t ypos = view_y + i;
            
            //don't draw virtual lines
            if(ypos >= n)break;
            
            auto &l = data->get(ypos);
            if(view_x < l.len())
            {
                IOLayer::moveCursor(0, i);
                
                size_t n = l.len() - view_x;
                IOLayer::writeChars(l.get() + view_x, n >= x_mmax ? x_mmax : n);
            }
        }
        IOLayer::moveCursor(x,y);
    }
    
    static void x_line(bool always_redraw=false)
    {
        const size_t ypos = view_y + y;
        if(ypos < data->size())
        {
            size_t pos = min<size_t>(data->get(ypos).len(), lpos);
            
            if(pos < view_x)
            {   // too far behind the line
                view_x = pos;
                x = 0;
                redraw_full();
            }
            else if(pos > (view_x + x_max))
            {   // too far ahead of the line
                view_x = pos - x_max;
                x = x_max;
                redraw_full();
            }
            else
            {
                x = pos - view_x;
                if(always_redraw) redraw_full();
            }
        }
        else
        {
            x = 0;
            view_x = 0;
            if(always_redraw) redraw_full();
        }
    }
    
    static void y_plus()
    {
        const size_t ypos = (y+view_y);
        if(virtual_whitespace || (data->size() > 0u && ypos < (data->size()-1u)))
        {
            if(y < y_max)
            {
                y++;
                x_line();
            }
            else
            {
                view_y++;
                x_line(true);
            }
        }
        else
        {
            lpos = data->get(ypos).len();
            x_line();
        }
        IOLayer::moveCursor(x, y);
    }
    
    static void y_minus()
    {
        if(y > y_min)
        {
            y--;
            x_line();
        }
        else if(view_y > 0)
        {
            view_y--;
            x_line(true);
        }
        else // if(y == y_min && view_y == 0)
        {
            x = 0;
            view_x = 0;
        }
        IOLayer::moveCursor(x,y);
    }
    
    static void x_plus(){
        const size_t ypos = (view_y + y);
        const size_t xpos = (view_x + x);
        
        if(data->size() > ypos && (data->get(ypos).len() > xpos))
        {
            x++;
        }
        else
        {
            if(virtual_whitespace || (data->size() > 0 && ypos< (data->size()-1)))
            {
                x = 0;
                if(y < y_max)
                {
                    y++;
                    if(view_x != 0)
                    {
                        view_x = 0;
                        redraw_full();
                    }
                }
                else
                {
                    view_y++;
                    view_x = 0;
                    redraw_full();
                }
            }
        }
        
        lpos = view_x + x;
        IOLayer::moveCursor(x, y);
    }
    
    static void x_minus(){
        if(x > x_min)
        {
            x--;
        }
        else if(view_x > 0)
        {
            view_x--;
            redraw_full();
        }
        else if(y > y_min)
        {
            y--;
            if((view_y + y) < data->size())
            {
                lpos = data->get(view_y + y).len();
            }
            x_line();
        }
        lpos = view_x + x;
        IOLayer::moveCursor(x, y);
    }
    
    static void write(char c)
    {
        const size_t xpos = view_x + x;
        auto &l = data->get(y + view_y);
        
        if(xpos < l.len())
        {
            l.insert(c, xpos);
            x_plus();
            redraw_line(y);
        }
        else
        {
            l.insert(c, xpos);
            IOLayer::writeChar(c);
            x_plus();
        }
    }
    
    static void erase(){
        if(x == 0)
        {
            if(y > 0)
            {
                const size_t ypos = view_y + y;
                if(ypos < data->size())
                {
                    auto &prev_line = data->get(ypos - 1);
                    lpos = prev_line.len();
                    auto &cur_line = data->get(ypos);
                    prev_line.append(cur_line);
                    data->erase(ypos);
                    y_minus();
                    x_line(true);
                }
                else
                {
                    lpos = 0;
                    y_minus();
                }
            }
        }
        else
        {
            x_minus();
            auto & cur_line = data->get(view_y + y);
            cur_line.erase(view_x + x);
            redraw_line(y);
        }
    }
    
    void newline()
    {
        const size_t ypos = view_y + y;
        if(!virtual_whitespace || ypos < data->size())
        {
            data->insert(ypos + 1,data->get(ypos).split(view_x + x));
        }
        lpos = 0;
        y_plus();
        redraw_full();
    }
    
    void init(int16_t off, Text * t_data){
        x_min = 0;
        x = 0;
        x_max = 79;
        x_mmax = 80;
        y_min = 0;
        y = 0;
        y_max = 24 - off;
        y_mmax = 25 - off;
        view_x = 0;
        view_y = 0;
        lpos = 0;
        IOLayer::moveCursor(x, y);
        data = t_data;
    }
    
    
    void reInit(Text * t_data)
    {
        x = 0;
        y = 0;
        view_x = 0;
        view_y = 0;
        lpos = 0;
        IOLayer::moveCursor(x, y);
        
        data->~Text();
        free(data);
        
        data = t_data;
        redraw_full();
    }
    
    void set_offset(int16_t off){
        y_max = 24 - off;
        y_mmax = 25 - off;
        if(y == y_mmax) y_minus();
    }
    
    void handle_input(IOLayer::keypress key){
        switch(key.type){
            case IOLayer::ARROW_UP:
                y_minus();
                break;
            case IOLayer::ARROW_DOWN:
                y_plus();
                break;
            case IOLayer::ARROW_LEFT:
                x_minus();
                break;
            case IOLayer::ARROW_RIGHT:
                x_plus();
                break;
            case IOLayer::KEY:
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
                        newline();
                        break;
                    case '\t':
                        do{
                            write(' ');
                        }while((view_x+x)%4);
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
    
    int save(const char * filename){
        FILE * f=fopen(filename,"w");
        if(!f)return 0;
        for(size_t i=0;i<data->size();i++){
            fputs(data->get(i).get(),f);
            fputc('\n',f);
        }
        fclose(f);
        return 1;
    }
    
    void update_cursor(){
        IOLayer::moveCursor(x,y);
    }
    
    void initFile(int16_t off,const char * filename){
        x_min = 0;
        x = 0;
        x_max = 79;
        x_mmax = 80;
        y_min = 0;
        y = 0;
        y_max = 24 - off;
        y_mmax = 25 - off;
        view_x = 0;
        view_y = 0;
        lpos = 0;
        IOLayer::moveCursor(x, y);
        
        data = new(malloc(sizeof(Text))) Text();
        
        MenuEngine::setfile(filename);
        FILE * f = fopen(filename, "r");
        if(f)
        {
            int c;
            size_t i = 0;
            while((c = fgetc(f)) != EOF)
            {   //reading per-character is slow but should be ~fine~
                TextLine &line = data->get(i);
                if(c == '\n')
                {
                    i++;
                }
                else if(c == '\t')
                {
                    do
                    {   // why is this a loop ??????
                        line.insert(c,line.len());
                    }
                    while(line.len() % 4);
                }
                else if(isgraph(c) || c == ' ')
                {
                    line.insert(c,line.len());
                }
            }
            fclose(f);
        }
    }
    
    void reInitFile(const char * filename)
    {
        x = 0;
        y = 0;
        view_x = 0;
        view_y = 0;
        lpos = 0;
        IOLayer::moveCursor(x, y);
        
        data->~Text();
        free(data);
        
        data = new(malloc(sizeof(Text))) Text();
        
        MenuEngine::setfile(filename);
        FILE * f = fopen(filename, "r");
        if(f)
        {
            int c;
            size_t i = 0;
            while((c = fgetc(f)) != EOF)
            {   //reading per-character is slow but should be ~fine~
                TextLine &line = data->get(i);
                if(c == '\n')
                {
                    i++;
                }
                else if(c == '\t')
                {
                    do
                    {   // why is this a loop ??????
                        line.insert(c,line.len());
                    }
                    while(line.len() % 4);
                }
                else if(isgraph(c) || c == ' ')
                {
                    line.insert(c,line.len());
                }
            }
            fclose(f);
        }
        redraw_full();
    }
}
