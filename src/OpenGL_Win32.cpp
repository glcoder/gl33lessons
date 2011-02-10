#include "OpenGL.h"

void* GL::getProc(const char *name)
{
	void *entrypt = (void *)wglGetProcAddress(name);

	if (NULL == entrypt)
		LOG_ERROR("Loading extension '%s' fail (%d)\n", name, GetLastError());

	return entrypt;
}
