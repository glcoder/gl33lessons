#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <cstring>
#include <cstdarg>

#if defined(_MSC_VER)
#define LOG_DEBUG(str,...) Log::write("%s:%s:%d DEBUG " str, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(str,...) Log::write("%s:%s:%d ERROR " str, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_DEBUG(str,...) Log::write("%s:%s:%d DEBUG " str, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(str,...) Log::write("%s:%s:%d ERROR " str, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#endif

#define ASSERT(x)          assert(x)

namespace Log
{
	void create(const char *filename);
	void destroy();
	void write(const char *format, ...);
};

#endif /* LOG_H */

