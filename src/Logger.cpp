#include "Logger.h"

const int LOGGER_FILENAME_MAX              = 256;
char g_LoggerFileName[LOGGER_FILENAME_MAX] = "lesson.log";

void LoggerCreate(const char *fileName)
{
	FILE *output;

	memset(g_LoggerFileName, 0, LOGGER_FILENAME_MAX);
	strncpy(g_LoggerFileName, fileName, LOGGER_FILENAME_MAX - 1);

	if ((output = fopen(g_LoggerFileName, "w")) != NULL)
		fclose(output);
}

void LoggerDestroy()
{
	//
}

void LoggerWrite(const char *format, ...)
{
	va_list ap;
	FILE    *output;

	if ((output = fopen(g_LoggerFileName, "a+")) == NULL)
		return;

	va_start(ap, format);
	vfprintf(output, format, ap);
	va_end(ap);

	fclose(output);
}
