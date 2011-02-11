#ifndef COMMON_H
#define COMMON_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cassert>
#include <cmath>
#include <ctime>

#define ARRAYSIZE(a) (sizeof(a)/sizeof(*(a)))

typedef float float2[2];
typedef float float3[3];
typedef float float4[4];

#if defined(_MSC_VER)
typedef __int8           int8_t;
typedef unsigned __int8  uint8_t;
typedef __int16          int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32          int32_t;
typedef unsigned __int32 uint32_t;

#define snprintf _snprintf
#else
#include <stdint.h>
#endif

#include "Log.h"

#endif /* COMMON_H */

