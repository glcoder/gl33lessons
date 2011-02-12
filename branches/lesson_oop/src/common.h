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

inline void float2_set(float2 var, float x, float y)
{
	var[0] = x;
	var[1] = y;
}

inline void float3_set(float3 var, float x, float y, float z)
{
	var[0] = x;
	var[1] = y;
	var[2] = z;
}

inline void float4_set(float3 var, float x, float y, float z, float w)
{
	var[0] = x;
	var[1] = y;
	var[2] = z;
	var[3] = w;
}

inline void float2_copy(float2 dst, float2 src)
{
	dst[0] = src[0];
	dst[1] = src[1];
}

inline void float3_copy(float3 dst, float3 src)
{
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
}

inline void float4_copy(float4 dst, float4 src)
{
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
	dst[3] = src[3];
}

#endif /* COMMON_H */
