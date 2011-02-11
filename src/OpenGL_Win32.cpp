#include "OpenGL.h"

void* GL::getProcAddress(const char *name)
{
	void *proc = (void *)wglGetProcAddress(name);

	if (NULL == proc)
		LOG_ERROR("wglGetProcAddress('%s') fail (%d)\n", name, GetLastError());

	return proc;
}

