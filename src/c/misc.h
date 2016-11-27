#pragma once

#include "libs/magpebapp.h"

/////////////////////////////////////////////////////////////////////////////
/// Prints the free and used heap sizes to the app log, with or without an additional c-string argument to append.
/////////////////////////////////////////////////////////////////////////////
#define HEAP_LOG_0()       APP_LOG(APP_LOG_LEVEL_DEBUG, "Free Heap: %zu b, Used Heap: %zu b", heap_bytes_free(), heap_bytes_used())
#define HEAP_LOG_1(str)    if (str != NULL) { APP_LOG(APP_LOG_LEVEL_DEBUG, "Free Heap: %zu b, Used Heap: %zu b -- %s", heap_bytes_free(), heap_bytes_used(), str); }
#define GET_MACRO(_0, _1, NAME, ...) NAME
#define HEAP_LOG(...) GET_MACRO(_0, ##__VA_ARGS__, HEAP_LOG_1, HEAP_LOG_0)(__VA_ARGS__)


/////////////////////////////////////////////////////////////////////////////
/// Returns the length of the specified string, or 0 if NULL.
/////////////////////////////////////////////////////////////////////////////
#define strxlen(str) (str==NULL?0:strlen(str))


// Math routines

// JRB NOTE: Macros for overflow testing, courtesy of https://www.fefe.de/intof.html
#define MPA_HALF_MPA_MAX_SIGNED(type) ((type)1 << (sizeof(type)*8-2))
#define MPA_MAX_SIGNED(type) (MPA_HALF_MPA_MAX_SIGNED(type) - 1 + MPA_HALF_MPA_MAX_SIGNED(type))
#define MPA_MIN_SIGNED(type) (-1 - MPA_MAX_SIGNED(type))
#define MPA_MIN(type) ((type)-1 < 1?MPA_MIN_SIGNED(type):(type)0)
#define MPA_MAX(type) ((type)~MPA_MIN(type))


#define assign(dest,src) ({ \
  typeof(src) __x=(src); \
  typeof(dest) __y=__x; \
  (__x==__y && ((__x<1) == (__y<1))?(void)((dest)=__y),0:1); \
})


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing an addition operation.
/// This code largely borrowed from https://www.fefe.de/intof.html
/// 
/// @param[in,out]  c  The variable that will hold the sum
/// @param[in]      a  the augend of the addition operation
/// @param[in]      b  the addend of the addition operation
///
/// @return  Evaluates to 0 if a + b will not overflow c (success)
///          Evaluates to 1 if a + b will overflow c (error condition)
/////////////////////////////////////////////////////////////////////////////
#define ADD_OVERFLOW(c,a,b) ({ \
  typeof(a) __a=a; \
  typeof(b) __b=b; \
  (__b)<1 ? \
    ((MPA_MIN(typeof(c))-(__b)<=(__a))?assign(c,__a+__b):1) : \
    ((MPA_MAX(typeof(c))-(__b)>=(__a))?assign(c,__a+__b):1); \
})


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing an subtraction operation.
/// This code largely borrowed from https://www.fefe.de/intof.html
/// 
/// @param[in,out]  c  The variable that will hold the difference
/// @param[in]      a  the minuend of the subtraction operation
/// @param[in]      b  the subtrahend of the subtraction operation
///
/// @return  Evaluates to 0 if a - b will not overflow c (success)
///          Evaluates to 1 if a - b will overflow c (error condition)
/////////////////////////////////////////////////////////////////////////////
#define SUB_OVERFLOW(c,a,b) ({ \
  typeof(a) __a=a; \
  typeof(b) __b=b; \
  (__b)<1 ? \
    ((MPA_MAX(typeof(c))+(__b)>=(__a))?assign(c,__a-__b):1) : \
    ((MPA_MIN(typeof(c))+(__b)<=(__a))?assign(c,__a-__b):1); \
})


// Addition with overflow checking, largely borrwed from https://www.fefe.de/intof.html
// JRB NOTE: Pebble doesn't have the "typeof" GCC extension, so I defined functions to
// replace the macros from https://www.fefe.de/intof.html
MagPebApp_ErrCode u16add_u16_u16(uint16_t* sum, const uint16_t augend, const uint16_t addend);
MagPebApp_ErrCode u16add_u16_s16(uint16_t* sum, const uint16_t augend, const int16_t addend);
MagPebApp_ErrCode s16add_u16_u16(int16_t* sum, const uint16_t augend, const uint16_t addend);
MagPebApp_ErrCode s16add_u16_s16(int16_t* sum, const uint16_t augend, const int16_t addend);
MagPebApp_ErrCode u32add_u32_u32(uint32_t* sum, const uint32_t augend, const uint32_t addend);
MagPebApp_ErrCode u32add_u32_s32(uint32_t* sum, const uint32_t augend, const int32_t addend);
MagPebApp_ErrCode s32add_u32_u32(int32_t* sum, const uint32_t augend, const uint32_t addend);
MagPebApp_ErrCode s32add_u32_s32(int32_t* sum, const uint32_t augend, const int32_t addend);
MagPebApp_ErrCode s32add_s32_s32(int32_t* sum, const int32_t augend, const int32_t addend);
MagPebApp_ErrCode u32mult_u32_u32(uint32_t* product, const uint32_t multiplicand, const uint32_t multiplier);

MagPebApp_ErrCode s32mult_s32_s32(int32_t* product, const int32_t multiplicand, const int32_t multiplier);

MagPebApp_ErrCode s32RoundUp(int32_t* rounded, const int32_t numToRound, const int32_t multiple);


// Pebble UI functions
void textLayer_stylize(TextLayer*, const GColor, const GColor, const GTextAlignment, const GFont);

uint16_t rel2Pxl(const uint16_t max, const uint8_t pct);
// JRB NOTE: A couple of helper macros for succinct-looking code. As with all macros, use wisely.
#define RELH(MAX,PCT) (rel2Pxl(MAX.size.h, PCT))
#define RELW(MAX,PCT) (rel2Pxl(MAX.size.w, PCT))

// String functions
bool strxcpy(char* buffer, size_t bufsize, const char* source, const char* readable);
MagPebApp_ErrCode strxcpyalloc(char** dest, const char* src);
