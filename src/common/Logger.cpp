#include "Logger.h"

Logger *g_Logger = NULL;

Logger::Logger(const char *fileName)
{
	FILE *output;

	memset(m_fileName, 0, LOGGER_FILENAME_MAX);
	strncpy(m_fileName, fileName, LOGGER_FILENAME_MAX - 1);

	if ((output = fopen(m_fileName, "w")) != NULL)
		fclose(output);
}

void Logger::write(const char *format, ...) const
{
	va_list ap;
	FILE    *output;

	if ((output = fopen(m_fileName, "a+")) == NULL)
		return;

	va_start(ap, format);
	vfprintf(output, format, ap);
	va_end(ap);

	fclose(output);
}
