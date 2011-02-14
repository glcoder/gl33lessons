#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "VFS.h"
#include "OpenGL.h"

class Texture
{
public:
	Texture();
	~Texture();

	void create(GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR,
		GLint warp = GL_CLAMP_TO_EDGE, GLenum target = GL_TEXTURE_2D);

	void destroy();

	GLuint getHandle() const;

	void image2D(const GLvoid *data, GLsizei width, GLsizei height,
		GLint internalFormat, GLenum format, GLenum type, bool genMIPs = false);

	void image2DMultisample(GLsizei width, GLsizei height, GLint internalFormat,
		GLsizei samples, GLboolean fixedSampleLocations = GL_FALSE);

	void bind(GLint unit = 0, bool compareToRef = false) const;

	bool load2DTGA(const char *name, bool genMIPs = false);
	bool load2DPNG(const char *name, bool genMIPs = false);

protected:
	GLuint m_handle;
	GLenum m_target;
};

#endif /* TEXTURE_H */
