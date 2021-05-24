#pragma once

#include <cstdint>

#ifndef NDEBUG
    #if defined(_WIN32)
        #define DEBUGGER() __debugbreak()
    #elif defined(__linux__)
        #include <signal.h>
        #define DEBUGGER() raise(SIGTRAP)
    #endif
#endif

#ifndef DEBUGGER
    #define DEBUGGER()
#endif

#define PI 3.14159265358979323846f

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;
typedef float f32;
typedef double f64;

typedef union hmm_vec2 v2;
typedef union hmm_vec3 v3;
typedef union hmm_vec4 v4;
typedef union hmm_mat4 m4;
