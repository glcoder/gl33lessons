#include "Shader.h"

static GLint ShaderProgramStatus(GLuint program, GLenum param)
{
	GLint status, length;
	GLchar buffer[1024];

	OPENGL_CALL(glGetProgramiv(program, param, &status));

	if (status != GL_TRUE)
	{
		OPENGL_CALL(glGetProgramInfoLog(program, 1024, &length, buffer));
		LOG_ERROR("Shader program: %s\n", (const char*)buffer);
	}

	return status;
}

static GLint ShaderStatus(GLuint shader, GLenum param)
{
	GLint status, length;
	GLchar buffer[1024];

	OPENGL_CALL(glGetShaderiv(shader, param, &status));

	if (status != GL_TRUE)
	{
		OPENGL_CALL(glGetShaderInfoLog(shader, 1024, &length, buffer));
		LOG_ERROR("Shader: %s\n", (const char*)buffer);
	}

	return status;
}

// Shader program
GLuint ShaderProgramCreate()
{
	GLuint program;

	OPENGL_CALL(program = glCreateProgram());

	return program;
}

void ShaderProgramDestroy(GLuint program)
{
	OPENGL_CALL(glDeleteProgram(program));
}

void ShaderProgramUse(GLuint program)
{
	OPENGL_CALL(glUseProgram(program));
}

bool ShaderProgramLink(GLuint program)
{
	OPENGL_CALL(glLinkProgram(program));
	return ShaderProgramStatus(program, GL_LINK_STATUS) == GL_TRUE;
}

bool ShaderProgramValidate(GLuint program)
{
	OPENGL_CALL(glValidateProgram(program));
	return ShaderProgramStatus(program, GL_VALIDATE_STATUS) == GL_TRUE;
}

void ShaderProgramAttach(GLuint program, GLuint shader)
{
	OPENGL_CALL(glAttachShader(program, shader));
}

void ShaderProgramDetach(GLuint program, GLuint shader)
{
	OPENGL_CALL(glDetachShader(program, shader));
}

// Shaders
GLuint ShaderCreate(GLuint type)
{
	GLuint shader;

	OPENGL_CALL(shader = glCreateShader(type));

	return shader;
}

void ShaderDestroy(GLuint shader)
{
	OPENGL_CALL(glDeleteShader(shader));
}

void ShaderLoad(GLuint shader, const GLchar *data, GLint length)
{
	ASSERT(data);

	OPENGL_CALL(glShaderSource(shader, 1, &data, &length));
}

bool ShaderLoadFromFile(GLuint shader, const char *fileName)
{
	ASSERT(fileName);

	FILE   *input;
	GLchar *buffer;
	GLint  size, readed;
	bool   result;

	if ((input = fopen(fileName, "rb")) == NULL)
	{
		LOG_ERROR("Opening shader file '%s'\n", fileName);
		return false;
	}

	fseek(input, 0L, SEEK_END);
	size = (GLint)ftell(input);
	rewind(input);

	if (size == 0)
	{
		LOG_ERROR("Empty shader file '%s'\n", fileName);
		fclose(input);
		return false;
	}

	buffer = new GLchar[size];
	readed = (GLint)fread(buffer, 1, size, input);

	fclose(input);

	result = (readed == size);

	if (result)
	{
		ShaderLoad(shader, buffer, readed);
	} else
	{
		LOG_ERROR("Reading shader file '%s'\n", fileName);
	}

	delete[] buffer;

	return result;
}

bool ShaderCompile(GLuint shader)
{
	OPENGL_CALL(glCompileShader(shader));
	return ShaderStatus(shader, GL_COMPILE_STATUS) == GL_TRUE;
}
