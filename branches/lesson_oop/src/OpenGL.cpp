#include "OpenGL.h"

GLenum GL::error = GL_NO_ERROR;

GLenum GL::getError()
{
	return (error = glGetError());
}

void GL::information()
{
	GLint major, minor;

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	LOG_DEBUG("OpenGL render context information:\n"
		"  Renderer       : %s\n"
		"  Vendor         : %s\n"
		"  Version        : %s\n"
		"  GLSL version   : %s\n"
		"  OpenGL version : %d.%d\n",
		(const char*)glGetString(GL_RENDERER),
		(const char*)glGetString(GL_VENDOR),
		(const char*)glGetString(GL_VERSION),
		(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION),
		major, minor
	);

	GL_CHECK_FOR_ERRORS();
}

bool GL::initialize()
{
	return true;
}

