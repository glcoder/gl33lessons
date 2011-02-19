#ifndef LIGHT_H
#define LIGHT_H

#include "common.h"
#include "math/math3d.h"
#include "OpenGL.h"

enum LightType { LT_POINT = 0, LT_DIRECTIONAL, LT_SPOT };

struct Light
{
	LightType type;

	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  position;
	vec3  attenuation;
	vec3  spotDirection;
	float spotExponent;
	float spotCosCutoff;
};

void LightDefault(Light &light, LightType type);
void LightSetup(GLuint program, const Light &light);

#endif /* LIGHT_H */
