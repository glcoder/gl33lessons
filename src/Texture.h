#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "OpenGL.h"

GLuint TextureCreateFromTGA(const char *fileName);

void TextureDestroy(GLuint texture);

#endif /* TEXTURE_H */
