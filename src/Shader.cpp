#include "Shader.h"

#define DEFINE_TO_STR_HELPER(x) #x
#define DEFINE_TO_STR(x)        "#define " #x " " DEFINE_TO_STR_HELPER(x) "\n"

static const GLchar vertexShaderDefines[] =
	"#version 330 core\n\n"
	DEFINE_TO_STR(VERT_POSITION)
	DEFINE_TO_STR(VERT_NORMAL)
	DEFINE_TO_STR(VERT_TEXCOORD)
	DEFINE_TO_STR(VERT_TANGENT)
	DEFINE_TO_STR(VERT_BINORMAL)
	"\n";

static const GLchar fragmentShaderDefines[] =
	"#version 330 core\n\n"
	DEFINE_TO_STR(FRAG_COLOR)
	DEFINE_TO_STR(FRAG_NORMAL)
	"\n";

Shader::Shader():
	m_handle(0), m_type(0)
{
}

Shader::~Shader()
{
	destroy();
}

void Shader::create(GLenum type)
{
	ASSERT(m_handle == 0);

	m_type   = type;
	m_handle = glCreateShader(m_type);
}

void Shader::destroy()
{
	if (glIsShader(m_handle) == GL_TRUE)
		glDeleteShader(m_handle);

	m_handle = 0;
}

bool Shader::load(GLenum type, const char *name)
{
	ASSERT(name);

	uint8_t  *buffer;
	uint32_t size;

	if (!VFS::load(name, VFS_BINARY, &buffer, &size))
		return false;

	create(type);
	source((const GLchar *)buffer, (GLint)size);

	delete[] buffer;

	return compile();
}

void Shader::source(const GLchar *data, GLint length) const
{
	ASSERT(data);

	const GLchar *shaderSource[2] = {NULL, data};
	GLint        shaderLength[2]  = {0, length};

	if (m_type == GL_VERTEX_SHADER)
	{
		shaderSource[0] = vertexShaderDefines;
		shaderLength[0] = ARRAYSIZE(vertexShaderDefines) - 1;
	} else if (m_type == GL_FRAGMENT_SHADER)
	{
		shaderSource[0] = fragmentShaderDefines;
		shaderLength[0] = ARRAYSIZE(fragmentShaderDefines) - 1;
	}

	glShaderSource(m_handle, 2, shaderSource, shaderLength);
}

bool Shader::compile() const
{
	glCompileShader(m_handle);

	return getStatus(GL_COMPILE_STATUS) == GL_TRUE;
}

GLuint Shader::getHandle() const
{
	return m_handle;
}

GLenum Shader::getType() const
{
	return m_type;
}

GLint Shader::getStatus(GLenum param) const
{
	GLint  status, length;
	GLchar buffer[1024];

	glGetShaderiv(m_handle, param, &status);

	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(m_handle, ARRAYSIZE(buffer), &length, buffer);
		LOG_ERROR("Shader: %s\n", (const char *)buffer);
	}

	return status;
}

ShaderProgram::ShaderProgram():
	m_handle(0)
{
}

ShaderProgram::~ShaderProgram()
{
	destroy();
}

void ShaderProgram::create()
{
	ASSERT(m_handle == 0);

	m_handle = glCreateProgram();
}

void ShaderProgram::destroy()
{
	if (glIsProgram(m_handle) == GL_TRUE)
	{
		glUseProgram(0);
		glDeleteProgram(m_handle);
	}

	m_handle = 0;
}

void ShaderProgram::attach(const Shader &shader) const
{
	glAttachShader(m_handle, shader.getHandle());
}

bool ShaderProgram::link() const
{
	glLinkProgram(m_handle);

	return getStatus(GL_LINK_STATUS) == GL_TRUE;
}

bool ShaderProgram::validate() const
{
	glValidateProgram(m_handle);

	return getStatus(GL_VALIDATE_STATUS) == GL_TRUE;
}

void ShaderProgram::bind() const
{
	glUseProgram(m_handle);
}

GLuint ShaderProgram::getHandle() const
{
	return m_handle;
}

GLint ShaderProgram::getStatus(GLenum param) const
{
	GLint  status, length;
	GLchar buffer[1024];

	glGetProgramiv(m_handle, param, &status);

	if (status != GL_TRUE)
	{
		glGetProgramInfoLog(m_handle, ARRAYSIZE(buffer), &length, buffer);
		LOG_ERROR("Shader program: %s\n", (const char *)buffer);
	}

	return status;
}
