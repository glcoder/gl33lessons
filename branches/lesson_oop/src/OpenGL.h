#ifndef OPENGL_H
#define OPENGL_H

#include "common.h"

#include <GL/gl.h>
#include "GL/glext.h"

#ifdef _WIN32
#include "GL/wglext.h"
#endif

#define GL_GET_PROC(p, n) \
	n = (p)GL::getProc(#n)

#define GL_GET_PROC_CRITICAL(p, n) \
	n = (p)GL::getProc(#n); \
	if (NULL == n) \
		return false

#define GL_CHECK_FOR_ERRORS() \
	if (GL::getError() != GL_NO_ERROR) \
		LOG_ERROR("OpenGL error 0x%X\n", (unsigned)GL::error);

struct GL
{
	static GLenum error;

	static void*  getProc(const char *name);
	static GLenum getError();

	static bool initialize();
	static void information();
};

#endif /* OPENGL_H */
