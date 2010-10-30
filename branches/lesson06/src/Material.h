#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"
#include "math/math3d.h"
#include "OpenGL.h"
#include "Texture.h"

struct Material
{
	GLuint texture;

	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  emission;
	float shininess;
};

void MaterialDefault(Material &material);
void MaterialSetup(GLuint program, const Material &material);

#endif /* MATERIAL_H */
