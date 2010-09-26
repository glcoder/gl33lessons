#ifndef OPENGL_H
#define OPENGL_H

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include <GL/gl.h>
#include "GL/glext.h"
#include "GL/wglext.h"

#include "common.h"

// глобальная перменная для хранения ошибки OpenGL
extern GLenum g_OpenGLError;

// проверка на ошибки OpenGL
#define OPENGL_CHECK_FOR_ERRORS() \
	if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
		LOG_ERROR("OpenGL error %d\n", (int)g_OpenGLError);

// безопасный вызов функции OpenGL
#define OPENGL_CALL(expression) \
	{ \
		expression; \
		if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
			LOG_ERROR("OpenGL expression \"" #expression "\" error %d\n", (int)g_OpenGLError); \
	}

#endif /* OPENGL_H */
