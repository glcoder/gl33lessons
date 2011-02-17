#include "Framebuffer.h"

Framebuffer::Framebuffer():
	m_handle(0)
{
}

Framebuffer::~Framebuffer()
{
	destroy();
}

void Framebuffer::create()
{
	ASSERT(m_handle == 0);

	glGenFramebuffers(1, &m_handle);
}

void Framebuffer::destroy()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &m_handle);
	m_handle = 0;
}

GLuint Framebuffer::getHandle() const
{
	return m_handle;
}

void Framebuffer::attach(GLenum target, GLenum attachment,
	const Texture &texture) const
{
	glBindFramebuffer(target, m_handle);
	glFramebufferTexture(target, attachment, texture.getHandle(), 0);
}

bool Framebuffer::complete(GLenum target) const
{
	GLenum status = glCheckFramebufferStatus(target);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		LOG_ERROR("Framebuffer error 0x%X\n", status);

	return (status == GL_FRAMEBUFFER_COMPLETE);
}

void Framebuffer::bind(GLenum target) const
{
	glBindFramebuffer(target, m_handle);
}

void Framebuffer::blit(const Framebuffer &dst, GLint srcX0, GLint srcY0,
	GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0,
	GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_handle);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.getHandle());

	glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1,
		dstX0, dstY0, dstX1, dstY1, mask, filter);
}

void Framebuffer::setDrawBuffer(GLenum mode) const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_handle);
	glDrawBuffer(mode);
}

void Framebuffer::setReadBuffer(GLenum mode) const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_handle);
	glReadBuffer(mode);
}
