#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define AssertNew(ptr) { if (ptr == NULL) { assert(false); abort(); } }

#ifdef _WIN32
#  define INLINE inline
#else
#  define INLINE static inline
#endif

#ifndef M_PI
#define M_PI 3.1415926535897931
#endif
#define DEG_TO_RAD (M_PI / 180.0)

INLINE char *Strdup(const char *src)
{
#ifdef _WIN32
    return _strdup(src);
#else
    return strdup(src);
#endif
}

INLINE void Memcpy(
    void *destination, long long destinationSize,
    const void *source, long long sourceSize)
{
#ifdef _WIN32
    memcpy_s(destination, destinationSize, source, sourceSize);
#else
    (void)destinationSize;
    memcpy(destination, source, sourceSize);
#endif
}
