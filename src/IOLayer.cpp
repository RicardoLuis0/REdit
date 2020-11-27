#include "IOLayer.h"

#ifdef _WIN32

#include "IOLayerWin32.cpp"

#elif defined(__unix__ )

#include "IOLayerNix.cpp"

#elif defined(__OSDEV_V2__)

#include "IOLayerOsDevV2.cpp"

#endif
