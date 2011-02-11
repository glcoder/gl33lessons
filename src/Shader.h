#ifndef SHADER_H
#define SHADER_H

#include "common.h"
#include "OpenGL.h"
#include "VFS.h"

class Shader
{
public:
	Shader();
	~Shader();

	void create(GLenum type);
	void destroy();

	bool create(GLenum type, const char *name);

	void source(const GLchar *data, GLint length) const;
	bool compile() const;

	GLuint getHandle() const;
	GLenum getType() const;

protected:
	GLint getStatus(GLenum param) const;

	GLuint m_handle;
	GLenum m_type;
};


class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	void create();
	void destroy();

	void attach(const Shader &shader) const;

	bool link() const;
	bool validate() const;

	void bind() const;

	GLuint getHandle() const;

protected:
	GLint getStatus(GLenum param) const;

	GLuint m_handle;
};

#endif /* SHADER_H */

