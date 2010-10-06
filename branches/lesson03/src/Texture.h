#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "OpenGL.h"

// создает текстуру из TGA-файла с изображением
GLuint TextureCreateFromTGA(const char *fileName);

// удаляет текстуру
void TextureDestroy(GLuint texture);

#endif /* TEXTURE_H */
