#pragma once
#include <stddef.h>

namespace TEdit {
    template<typename T> constexpr T max(T a,T b){
        return a>b?a:b;
    }
    
    template<typename T> constexpr T min(T a,T b){
        return a>b?b:a;
    }
}

// placement new
inline void* operator new(size_t, void *p) {
    return p;
}
