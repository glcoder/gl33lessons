#ifndef OPENGL_H
#define OPENGL_H

#include "common.h"

#include <GL/gl.h>
#include "GL/glext.h"

#ifdef _WIN32
#include "GL/wglext.h"
#endif

#define VERT_POSITION  0
#define VERT_TEXCOORD  1
#define VERT_NORMAL    2

#define FRAG_OUTPUT0   0

#define GL_OFFSET(x) ((const GLvoid *)(x))

#define GL_GET_PROC(p, n) \
	n = (p)GL::getProcAddress(#n)

#define GL_GET_PROC_CRITICAL(p, n) \
	n = (p)GL::getProcAddress(#n); \
	if (NULL == n) \
		return false

#define GL_CHECK_FOR_ERRORS() \
	if (GL::getError() != GL_NO_ERROR) \
		LOG_ERROR("OpenGL error 0x%X\n", (unsigned)GL::error);

namespace GL
{
	GLenum error;

	void*  getProcAddress(const char *name);
	GLenum getError();

	bool initialize();
	void information();
};

#endif /* OPENGL_H */

