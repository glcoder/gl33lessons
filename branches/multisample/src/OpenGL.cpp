#include "OpenGL.h"

GLenum g_OpenGLError = GL_NO_ERROR;

GLint ShaderProgramStatus(GLuint program, GLenum param)
{
	GLint status, length;
	GLchar buffer[1024];

	glGetProgramiv(program, param, &status);

	if (status != GL_TRUE)
	{
		glGetProgramInfoLog(program, 1024, &length, buffer);
		LOG_ERROR("Shader program: %s\n", (const char*)buffer);
	}

	OPENGL_CHECK_FOR_ERRORS();

	return status;
}

GLint ShaderStatus(GLuint shader, GLenum param)
{
	GLint status, length;
	GLchar buffer[1024];

	glGetShaderiv(shader, param, &status);

	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(shader, 1024, &length, buffer);
		LOG_ERROR("Shader: %s\n", (const char*)buffer);
	}

	OPENGL_CHECK_FOR_ERRORS();

	return status;
}

bool OpenGLInitExtensions()
{
	// Texture
	OPENGL_GET_PROC(PFNGLACTIVETEXTUREPROC, glActiveTexture);
	// VAO
	OPENGL_GET_PROC(PFNGLGENVERTEXARRAYSPROC,    glGenVertexArrays);
	OPENGL_GET_PROC(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
	OPENGL_GET_PROC(PFNGLBINDVERTEXARRAYPROC,    glBindVertexArray);
	// VBO
	OPENGL_GET_PROC(PFNGLGENBUFFERSPROC,    glGenBuffers);
	OPENGL_GET_PROC(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
	OPENGL_GET_PROC(PFNGLBINDBUFFERPROC,    glBindBuffer);
	OPENGL_GET_PROC(PFNGLBUFFERDATAPROC,    glBufferData);
	OPENGL_GET_PROC(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
	// Shaders
	OPENGL_GET_PROC(PFNGLCREATEPROGRAMPROC,     glCreateProgram);
	OPENGL_GET_PROC(PFNGLDELETEPROGRAMPROC,     glDeleteProgram);
	OPENGL_GET_PROC(PFNGLLINKPROGRAMPROC,       glLinkProgram);
	OPENGL_GET_PROC(PFNGLVALIDATEPROGRAMPROC,   glValidateProgram);
	OPENGL_GET_PROC(PFNGLUSEPROGRAMPROC,        glUseProgram);
	OPENGL_GET_PROC(PFNGLGETPROGRAMIVPROC,      glGetProgramiv);
	OPENGL_GET_PROC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
	OPENGL_GET_PROC(PFNGLCREATESHADERPROC,      glCreateShader);
	OPENGL_GET_PROC(PFNGLDELETESHADERPROC,      glDeleteShader);
	OPENGL_GET_PROC(PFNGLSHADERSOURCEPROC,      glShaderSource);
	OPENGL_GET_PROC(PFNGLCOMPILESHADERPROC,     glCompileShader);
	OPENGL_GET_PROC(PFNGLATTACHSHADERPROC,      glAttachShader);
	OPENGL_GET_PROC(PFNGLDETACHSHADERPROC,      glDetachShader);
	OPENGL_GET_PROC(PFNGLGETSHADERIVPROC,       glGetShaderiv);
	OPENGL_GET_PROC(PFNGLGETSHADERINFOLOGPROC,  glGetShaderInfoLog);
	// Shaders attributes
	OPENGL_GET_PROC(PFNGLGETATTRIBLOCATIONPROC,        glGetAttribLocation);
	OPENGL_GET_PROC(PFNGLVERTEXATTRIBPOINTERPROC,      glVertexAttribPointer);
	OPENGL_GET_PROC(PFNGLENABLEVERTEXATTRIBARRAYPROC,  glEnableVertexAttribArray);
	OPENGL_GET_PROC(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);
	// Shaders uniforms
	OPENGL_GET_PROC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
	OPENGL_GET_PROC(PFNGLUNIFORMMATRIX4FVPROC,   glUniformMatrix4fv);
	OPENGL_GET_PROC(PFNGLUNIFORM1IPROC,          glUniform1i);

	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

// объявим расширения OpenGL
// Texture
PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
// VAO
PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays    = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC    glBindVertexArray    = NULL;
// VBO
PFNGLGENBUFFERSPROC    glGenBuffers    = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLBINDBUFFERPROC    glBindBuffer    = NULL;
PFNGLBUFFERDATAPROC    glBufferData    = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
// Shaders
PFNGLCREATEPROGRAMPROC     glCreateProgram     = NULL;
PFNGLDELETEPROGRAMPROC     glDeleteProgram     = NULL;
PFNGLLINKPROGRAMPROC       glLinkProgram       = NULL;
PFNGLVALIDATEPROGRAMPROC   glValidateProgram   = NULL;
PFNGLUSEPROGRAMPROC        glUseProgram        = NULL;
PFNGLGETPROGRAMIVPROC      glGetProgramiv      = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLCREATESHADERPROC      glCreateShader      = NULL;
PFNGLDELETESHADERPROC      glDeleteShader      = NULL;
PFNGLSHADERSOURCEPROC      glShaderSource      = NULL;
PFNGLCOMPILESHADERPROC     glCompileShader     = NULL;
PFNGLATTACHSHADERPROC      glAttachShader      = NULL;
PFNGLDETACHSHADERPROC      glDetachShader      = NULL;
PFNGLGETSHADERIVPROC       glGetShaderiv       = NULL;
PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog  = NULL;
// Shaders attributes
PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation        = NULL;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer      = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray  = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
// Shaders uniforms
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv   = NULL;
PFNGLUNIFORM1IPROC          glUniform1i          = NULL;
