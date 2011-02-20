#ifndef OPENGL_H
#define OPENGL_H

#include "common.h"

#include <GL/gl.h>
#include "GL/glext.h"

#ifdef _WIN32
#include "GL/wglext.h"
#endif

#define VERT_POSITION  0
#define VERT_TEXCOORD  1
#define VERT_NORMAL    2
#define VERT_TANGENT   3
#define VERT_BINORMAL  4

#define FRAG_COLOR   0
#define FRAG_NORMAL  1

#define GL_PVOID(x) ((const GLvoid *)(x))

#define GL_GET_PROC(p, n) \
	n = (p)GL::getProcAddress(#n)

#define GL_GET_PROC_CRITICAL(p, n) \
	n = (p)GL::getProcAddress(#n); \
	if (NULL == n) \
		return false

#define GL_CHECK_FOR_ERRORS() \
	if (GL::getError() != GL_NO_ERROR) \
		LOG_ERROR("OpenGL error 0x%X\n", (unsigned)GL::error);

namespace GL
{
	extern GLenum error;

	void*  getProcAddress(const char *name);
	GLenum getError();

	bool initialize();
	void information();
}

extern PFNGLDRAWBUFFERSPROC              glDrawBuffers;
extern PFNGLACTIVETEXTUREPROC            glActiveTexture;
extern PFNGLGENERATEMIPMAPPROC           glGenerateMipmap;
extern PFNGLTEXIMAGE2DMULTISAMPLEPROC    glTexImage2DMultisample;
extern PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays;
extern PFNGLISVERTEXARRAYPROC            glIsVertexArray;
extern PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;
extern PFNGLGENBUFFERSPROC               glGenBuffers;
extern PFNGLDELETEBUFFERSPROC            glDeleteBuffers;
extern PFNGLISBUFFERPROC                 glIsBuffer;
extern PFNGLBINDBUFFERPROC               glBindBuffer;
extern PFNGLBUFFERDATAPROC               glBufferData;
extern PFNGLBUFFERSUBDATAPROC            glBufferSubData;
extern PFNGLMAPBUFFERPROC                glMapBuffer;
extern PFNGLUNMAPBUFFERPROC              glUnmapBuffer;
extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
extern PFNGLISPROGRAMPROC                glIsProgram;
extern PFNGLLINKPROGRAMPROC              glLinkProgram;
extern PFNGLVALIDATEPROGRAMPROC          glValidateProgram;
extern PFNGLUSEPROGRAMPROC               glUseProgram;
extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
extern PFNGLCREATESHADERPROC             glCreateShader;
extern PFNGLDELETESHADERPROC             glDeleteShader;
extern PFNGLISSHADERPROC                 glIsShader;
extern PFNGLSHADERSOURCEPROC             glShaderSource;
extern PFNGLCOMPILESHADERPROC            glCompileShader;
extern PFNGLATTACHSHADERPROC             glAttachShader;
extern PFNGLDETACHSHADERPROC             glDetachShader;
extern PFNGLGETSHADERIVPROC              glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
extern PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
extern PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
extern PFNGLUNIFORM1IPROC                glUniform1i;
extern PFNGLUNIFORM1FVPROC               glUniform1fv;
extern PFNGLUNIFORM3FVPROC               glUniform3fv;
extern PFNGLUNIFORM4FVPROC               glUniform4fv;
extern PFNGLBINDFRAMEBUFFERPROC          glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC       glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC          glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC   glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTUREPROC       glFramebufferTexture;
extern PFNGLBLITFRAMEBUFFERPROC          glBlitFramebuffer;

#endif /* OPENGL_H */
