#include "Log.h"

static const int LOG_FILENAME_MAX           = 256;
static char g_logFilename[LOG_FILENAME_MAX] = "app.log";

void Log::create(const char *filename)
{
	FILE *output;

	memset(g_logFilename, 0, LOG_FILENAME_MAX);
	strncpy(g_logFilename, filename, LOG_FILENAME_MAX);

	if ((output = fopen(g_logFilename, "w")) != NULL)
		fclose(output);
}

void Log::destroy()
{
	//
}

void Log::write(const char *format, ...)
{
	va_list ap;
	FILE    *output;

	if ((output = fopen(g_logFilename, "a+")) == NULL)
		return;

	va_start(ap, format);
	vfprintf(output, format, ap);
	va_end(ap);

	fclose(output);
}

