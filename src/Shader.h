#ifndef SHADER_H
#define SHADER_H

#include "common.h"
#include "Logger.h"
#include "OpenGL.h"

// Shader program
GLuint ShaderProgramCreate();
void ShaderProgramDestroy(GLuint program);
void ShaderProgramUse(GLuint program);
bool ShaderProgramLink(GLuint program);
bool ShaderProgramValidate(GLuint program);
void ShaderProgramAttach(GLuint program, GLuint shader);
void ShaderProgramDetach(GLuint program, GLuint shader);

// Shaders
GLuint ShaderCreate(GLuint type);
void ShaderDestroy(GLuint shader);
void ShaderLoad(GLuint shader, const GLchar *data, GLint length);
bool ShaderLoadFromFile(GLuint shader, const char *fileName);
bool ShaderCompile(GLuint shader);

#endif /* SHADER_H */
