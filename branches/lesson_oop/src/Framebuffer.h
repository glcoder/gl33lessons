#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "common.h"
#include "OpenGL.h"
#include "Texture.h"

class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();

	void create();
	void destroy();

	GLuint getHandle() const;

	void attach(GLenum target, GLenum attachment, const Texture &texture) const;

	bool complete(GLenum target = GL_FRAMEBUFFER) const;

	void bind(GLenum target = GL_FRAMEBUFFER) const;

	void blit(const Framebuffer &dst, GLint srcX0, GLint srcY0,
		GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0,
		GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) const;

	void setDrawBuffer(GLenum mode) const;
	void setReadBuffer(GLenum mode) const;

protected:
	GLuint m_handle;
};

#endif /* FRAMEBUFFER_H */
