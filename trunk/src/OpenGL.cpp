#include "OpenGL.h"

#define GL_INT_PRINT_DEBUG(name) \
	GLint info_ ## name; \
	glGetIntegerv(name, &info_ ## name); \
	LOG_DEBUG(#name " = %d\n", info_ ## name)

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

	//GL_INT_PRINT_DEBUG(GL_MAX_DRAW_BUFFERS);
	//GL_INT_PRINT_DEBUG(GL_MAX_COLOR_ATTACHMENTS);
	//GL_INT_PRINT_DEBUG(GL_MAX_SAMPLES);
	//GL_INT_PRINT_DEBUG(GL_MAX_COLOR_TEXTURE_SAMPLES);
	//GL_INT_PRINT_DEBUG(GL_MAX_DEPTH_TEXTURE_SAMPLES);

	GL_CHECK_FOR_ERRORS();
}

bool GL::initialize()
{
	GL_GET_PROC_CRITICAL(PFNGLDRAWBUFFERSPROC,              glDrawBuffers);
	GL_GET_PROC_CRITICAL(PFNGLACTIVETEXTUREPROC,            glActiveTexture);
	GL_GET_PROC_CRITICAL(PFNGLGENERATEMIPMAPPROC,           glGenerateMipmap);
	GL_GET_PROC_CRITICAL(PFNGLTEXIMAGE2DMULTISAMPLEPROC,    glTexImage2DMultisample);
	GL_GET_PROC_CRITICAL(PFNGLGENVERTEXARRAYSPROC,          glGenVertexArrays);
	GL_GET_PROC_CRITICAL(PFNGLDELETEVERTEXARRAYSPROC,       glDeleteVertexArrays);
	GL_GET_PROC_CRITICAL(PFNGLISVERTEXARRAYPROC,            glIsVertexArray);
	GL_GET_PROC_CRITICAL(PFNGLBINDVERTEXARRAYPROC,          glBindVertexArray);
	GL_GET_PROC_CRITICAL(PFNGLGENBUFFERSPROC,               glGenBuffers);
	GL_GET_PROC_CRITICAL(PFNGLDELETEBUFFERSPROC,            glDeleteBuffers);
	GL_GET_PROC_CRITICAL(PFNGLISBUFFERPROC,                 glIsBuffer);
	GL_GET_PROC_CRITICAL(PFNGLBINDBUFFERPROC,               glBindBuffer);
	GL_GET_PROC_CRITICAL(PFNGLBUFFERDATAPROC,               glBufferData);
	GL_GET_PROC_CRITICAL(PFNGLBUFFERSUBDATAPROC,            glBufferSubData);
	GL_GET_PROC_CRITICAL(PFNGLMAPBUFFERPROC,                glMapBuffer);
	GL_GET_PROC_CRITICAL(PFNGLUNMAPBUFFERPROC,              glUnmapBuffer);
	GL_GET_PROC_CRITICAL(PFNGLCREATEPROGRAMPROC,            glCreateProgram);
	GL_GET_PROC_CRITICAL(PFNGLDELETEPROGRAMPROC,            glDeleteProgram);
	GL_GET_PROC_CRITICAL(PFNGLISPROGRAMPROC,                glIsProgram);
	GL_GET_PROC_CRITICAL(PFNGLLINKPROGRAMPROC,              glLinkProgram);
	GL_GET_PROC_CRITICAL(PFNGLVALIDATEPROGRAMPROC,          glValidateProgram);
	GL_GET_PROC_CRITICAL(PFNGLUSEPROGRAMPROC,               glUseProgram);
	GL_GET_PROC_CRITICAL(PFNGLGETPROGRAMIVPROC,             glGetProgramiv);
	GL_GET_PROC_CRITICAL(PFNGLGETPROGRAMINFOLOGPROC,        glGetProgramInfoLog);
	GL_GET_PROC_CRITICAL(PFNGLCREATESHADERPROC,             glCreateShader);
	GL_GET_PROC_CRITICAL(PFNGLDELETESHADERPROC,             glDeleteShader);
	GL_GET_PROC_CRITICAL(PFNGLISSHADERPROC,                 glIsShader);
	GL_GET_PROC_CRITICAL(PFNGLSHADERSOURCEPROC,             glShaderSource);
	GL_GET_PROC_CRITICAL(PFNGLCOMPILESHADERPROC,            glCompileShader);
	GL_GET_PROC_CRITICAL(PFNGLATTACHSHADERPROC,             glAttachShader);
	GL_GET_PROC_CRITICAL(PFNGLDETACHSHADERPROC,             glDetachShader);
	GL_GET_PROC_CRITICAL(PFNGLGETSHADERIVPROC,              glGetShaderiv);
	GL_GET_PROC_CRITICAL(PFNGLGETSHADERINFOLOGPROC,         glGetShaderInfoLog);
	GL_GET_PROC_CRITICAL(PFNGLGETATTRIBLOCATIONPROC,        glGetAttribLocation);
	GL_GET_PROC_CRITICAL(PFNGLVERTEXATTRIBPOINTERPROC,      glVertexAttribPointer);
	GL_GET_PROC_CRITICAL(PFNGLENABLEVERTEXATTRIBARRAYPROC,  glEnableVertexAttribArray);
	GL_GET_PROC_CRITICAL(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);
	GL_GET_PROC_CRITICAL(PFNGLGETUNIFORMLOCATIONPROC,       glGetUniformLocation);
	GL_GET_PROC_CRITICAL(PFNGLUNIFORMMATRIX3FVPROC,         glUniformMatrix3fv);
	GL_GET_PROC_CRITICAL(PFNGLUNIFORMMATRIX4FVPROC,         glUniformMatrix4fv);
	GL_GET_PROC_CRITICAL(PFNGLUNIFORM1IPROC,                glUniform1i);
	GL_GET_PROC_CRITICAL(PFNGLUNIFORM1FVPROC,               glUniform1fv);
	GL_GET_PROC_CRITICAL(PFNGLUNIFORM3FVPROC,               glUniform3fv);
	GL_GET_PROC_CRITICAL(PFNGLUNIFORM4FVPROC,               glUniform4fv);
	GL_GET_PROC_CRITICAL(PFNGLBINDFRAMEBUFFERPROC,          glBindFramebuffer);
	GL_GET_PROC_CRITICAL(PFNGLDELETEFRAMEBUFFERSPROC,       glDeleteFramebuffers);
	GL_GET_PROC_CRITICAL(PFNGLGENFRAMEBUFFERSPROC,          glGenFramebuffers);
	GL_GET_PROC_CRITICAL(PFNGLCHECKFRAMEBUFFERSTATUSPROC,   glCheckFramebufferStatus);
	GL_GET_PROC_CRITICAL(PFNGLFRAMEBUFFERTEXTUREPROC,       glFramebufferTexture);
	GL_GET_PROC_CRITICAL(PFNGLBLITFRAMEBUFFERPROC,          glBlitFramebuffer);

	return true;
}

PFNGLDRAWBUFFERSPROC              glDrawBuffers = NULL;
PFNGLACTIVETEXTUREPROC            glActiveTexture = NULL;
PFNGLGENERATEMIPMAPPROC           glGenerateMipmap = NULL;
PFNGLTEXIMAGE2DMULTISAMPLEPROC    glTexImage2DMultisample = NULL;
PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays = NULL;
PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays = NULL;
PFNGLISVERTEXARRAYPROC            glIsVertexArray = NULL;
PFNGLBINDVERTEXARRAYPROC          glBindVertexArray = NULL;
PFNGLGENBUFFERSPROC               glGenBuffers = NULL;
PFNGLDELETEBUFFERSPROC            glDeleteBuffers = NULL;
PFNGLISBUFFERPROC                 glIsBuffer = NULL;
PFNGLBINDBUFFERPROC               glBindBuffer = NULL;
PFNGLBUFFERDATAPROC               glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC            glBufferSubData = NULL;
PFNGLMAPBUFFERPROC                glMapBuffer = NULL;
PFNGLUNMAPBUFFERPROC              glUnmapBuffer = NULL;
PFNGLCREATEPROGRAMPROC            glCreateProgram = NULL;
PFNGLDELETEPROGRAMPROC            glDeleteProgram = NULL;
PFNGLISPROGRAMPROC                glIsProgram = NULL;
PFNGLLINKPROGRAMPROC              glLinkProgram = NULL;
PFNGLVALIDATEPROGRAMPROC          glValidateProgram = NULL;
PFNGLUSEPROGRAMPROC               glUseProgram = NULL;
PFNGLGETPROGRAMIVPROC             glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog = NULL;
PFNGLCREATESHADERPROC             glCreateShader = NULL;
PFNGLDELETESHADERPROC             glDeleteShader = NULL;
PFNGLISSHADERPROC                 glIsShader = NULL;
PFNGLSHADERSOURCEPROC             glShaderSource = NULL;
PFNGLCOMPILESHADERPROC            glCompileShader = NULL;
PFNGLATTACHSHADERPROC             glAttachShader = NULL;
PFNGLDETACHSHADERPROC             glDetachShader = NULL;
PFNGLGETSHADERIVPROC              glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog = NULL;
PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation = NULL;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation = NULL;
PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv = NULL;
PFNGLUNIFORM1IPROC                glUniform1i = NULL;
PFNGLUNIFORM1FVPROC               glUniform1fv = NULL;
PFNGLUNIFORM3FVPROC               glUniform3fv = NULL;
PFNGLUNIFORM4FVPROC               glUniform4fv = NULL;
PFNGLBINDFRAMEBUFFERPROC          glBindFramebuffer = NULL;
PFNGLDELETEFRAMEBUFFERSPROC       glDeleteFramebuffers = NULL;
PFNGLGENFRAMEBUFFERSPROC          glGenFramebuffers = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC   glCheckFramebufferStatus = NULL;
PFNGLFRAMEBUFFERTEXTUREPROC       glFramebufferTexture = NULL;
PFNGLBLITFRAMEBUFFERPROC          glBlitFramebuffer = NULL;
