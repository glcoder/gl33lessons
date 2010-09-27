#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <cstring>
#include <cstdarg>

const int LOGGER_FILENAME_MAX = 256;

#define LOG_OPEN(filename) g_Logger = new Logger(filename)
#define LOG_CLOSE()        delete g_Logger

#if defined(_MSC_VER)
#define LOG_DEBUG(str,...) g_Logger->write("%s:%s:%d DEBUG " str, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(str,...) g_Logger->write("%s:%s:%d ERROR " str, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#elif defined(__MINGW32__)
#define LOG_DEBUG(str,...) g_Logger->write("%s:%s:%d DEBUG " str, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(str,...) g_Logger->write("%s:%s:%d ERROR " str, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#endif

#define ASSERT(x)          assert(x)

class Logger
{
public:
	Logger(const char *fileName);

	void write(const char *format, ...) const;

private:
	char m_fileName[LOGGER_FILENAME_MAX];
};

extern Logger *g_Logger;

#endif /* LOGGER_H */
