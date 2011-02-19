#ifndef SHADER_H
#define SHADER_H

#include "common.h"
#include "OpenGL.h"
#include "Light.h"
#include "Material.h"
#include "Camera.h"

// типы шейдеров
#define ST_VERTEX     0x01
#define ST_FRAGMENT   0x02

// создает шейдерную программу и загружает шейдеры указанные в type из файла fileName
// вершинный шейдер загружается из файла fileName.vs, фрагментный fileName.fs
GLuint ShaderProgramCreateFromFile(const char *fileName, int type);

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
