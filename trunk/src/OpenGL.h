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
	while ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
		LOG_ERROR("OpenGL error 0x%X\n", (unsigned)g_OpenGLError);

bool OpenGLInitExtensions();

GLint ShaderProgramStatus(GLuint program, GLenum param);
GLint ShaderStatus(GLuint shader, GLenum param);

// расширения OpenGL
// VAO
extern PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC    glBindVertexArray;
// VBO
extern PFNGLGENBUFFERSPROC    glGenBuffers;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLBINDBUFFERPROC    glBindBuffer;
extern PFNGLBUFFERDATAPROC    glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
// Shaders
extern PFNGLCREATEPROGRAMPROC     glCreateProgram;
extern PFNGLDELETEPROGRAMPROC     glDeleteProgram;
extern PFNGLLINKPROGRAMPROC       glLinkProgram;
extern PFNGLVALIDATEPROGRAMPROC   glValidateProgram;
extern PFNGLUSEPROGRAMPROC        glUseProgram;
extern PFNGLGETPROGRAMIVPROC      glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLCREATESHADERPROC      glCreateShader;
extern PFNGLDELETESHADERPROC      glDeleteShader;
extern PFNGLSHADERSOURCEPROC      glShaderSource;
extern PFNGLCOMPILESHADERPROC     glCompileShader;
extern PFNGLATTACHSHADERPROC      glAttachShader;
extern PFNGLDETACHSHADERPROC      glDetachShader;
extern PFNGLGETSHADERIVPROC       glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog;
// Shaders attributes
extern PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
// Shaders uniforms
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv;

#endif /* OPENGL_H */
