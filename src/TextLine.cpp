#include "TextLine.h"
#include "IOLayer.h"
#include <cstdlib>
#include <cstring>
#include "Util.h"

TextLine::TextLine() {
    _len=0;
    _alloc=8;//initial allocation is 8 chars
    buf=(char*)calloc(8,sizeof(char));
    if(buf==nullptr){
        IO::exit_error("Out of memory");
    }
    buf[0]='\0';
}

TextLine::TextLine(char * data,size_t len,size_t alloc):buf(data),_len(len),_alloc(alloc){
}

TextLine::TextLine(TextLine && other):buf(other.buf),_len(other._len),_alloc(other._alloc){
    other.buf=nullptr;
    other._len=0;
    other._alloc=0;
}

TextLine& TextLine::operator=(TextLine && other){
    buf=other.buf;
    _len=other._len;
    _alloc=other._alloc;
    other.buf=nullptr;
    other._len=0;
    other._alloc=0;
    return *this;
}

TextLine::~TextLine() {
    if(buf){
        free(buf);
    }
}

size_t TextLine::len(){
    return _len;
}

TextLine TextLine::split(size_t at){
    IO::exit_error("TextLine::split unimplemented");
}

void TextLine::insert(char c,size_t at){
    if(at>=_len){
        IO::exit_error("invalid position for TextLine::insert, length is %d, requested position is %d",_len,at);
    }
    resize(_len+1,false);
    memmove(buf+at,buf+at+1,_len-at);
    buf[at]=c;
}

void TextLine::resize(size_t new_size,bool move_terminator){
    if(new_size==_len)return;
    if((new_size+1)>_alloc){
        grow(new_size+1);
    }
    if(move_terminator&&new_size>_len){
        memset(buf+_len,' ',new_size-_len);
    }
    if(move_terminator||new_size<_len){
        buf[new_size]='\0';
    }
    _len=new_size;
}

void TextLine::grow(size_t to){
    if(to<=_alloc)return;//only allow growth
    size_t ns=max(_alloc*2,to);//double or grow to necessary, whichever is largest
    buf=(char*)realloc(buf,ns);
    _alloc=ns;
}

void TextLine::trim(){
    if(_alloc>(_len+1)){
        buf=(char*)realloc(buf,_len+1);
        _alloc=_len+1;
    }
}

const char * TextLine::get(){
    return buf;
}

