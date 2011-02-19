#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <cstring>
#include <cstdarg>

#if defined(_MSC_VER)
#define LOG_DEBUG(str,...) LoggerWrite("%s:%s:%d DEBUG " str, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(str,...) LoggerWrite("%s:%s:%d ERROR " str, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#elif defined(__MINGW32__)
#define LOG_DEBUG(str,...) LoggerWrite("%s:%s:%d DEBUG " str, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(str,...) LoggerWrite("%s:%s:%d ERROR " str, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#endif

#define ASSERT(x)          assert(x)

void LoggerCreate(const char *fileName);

void LoggerDestroy();

void LoggerWrite(const char *format, ...);

#endif /* LOGGER_H */
