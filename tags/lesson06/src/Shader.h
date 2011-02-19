#ifndef SHADER_H
#define SHADER_H

#include "common.h"
#include "OpenGL.h"
#include "Light.h"
#include "Material.h"
#include "Camera.h"

// создает шейдерную программу, загружает шейдеры из файлов и собирает шейдерную программу
GLuint ShaderProgramCreateFromFile(const char *vsName, const char *fsName);

// делает указанную шейдерную программу неактивной и удаляет ее
void ShaderProgramDestroy(GLuint program);

// собирает созданную и загруженную шейдерную программу
bool ShaderProgramLink(GLuint program);

// проверяет собранную шейдерную программу на корректность
bool ShaderProgramValidate(GLuint program);

// делает указанную шейдерную программу активной
void ShaderProgramBind(GLuint program);

// делает текущую шейдерную программу неактивной
void ShaderProgramUnbind();

#endif /* SHADER_H */
