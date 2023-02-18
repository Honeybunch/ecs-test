#pragma once

#include <stdint.h>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#endif

#ifdef TRACY_ENABLE
#include <tracy/TracyC.h>
#endif

#define TracyCategoryColorCore 0xe066ff
#define TracyCategoryColorRendering 0x7fff00
#define TracyCategoryColorUI 0xe0eeee
#define TracyCategoryColorMath 0xffdab9
#define TracyCategoryColorInput 0xffb5c5
#define TracyCategoryColorMemory 0xff8c69
#define TracyCategoryColorWait 0xff0000
#define TracyCategoryColorGame 0x0022ff

#ifdef __clang__
#pragma clang diagnostic pop
#endif
