#include "Light.h"

void LightDefault(Light &light, LightType type)
{
	light.type = type;

	light.ambient.set(0.0f, 0.0f, 0.0f, 1.0f);
	light.diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
	light.specular.set(1.0f, 1.0f, 1.0f, 1.0f);
	light.position.set(0.0f, 0.0f, 1.0f, 0.0f);
	light.attenuation.set(1.0f, 0.0f, 0.0f);
	light.spotDirection.set(0.0f, 0.0f, -1.0f);
	light.spotExponent = 0.0f;
	light.spotCosCutoff = -1.0f; // cos 180 градусов == -1
}

void LightSetup(GLuint program, const Light &light)
{
	glUniform4fv(glGetUniformLocation(program, "light.ambient"), 1, light.ambient.v);
	glUniform4fv(glGetUniformLocation(program, "light.diffuse"), 1, light.diffuse.v);
	glUniform4fv(glGetUniformLocation(program, "light.specular"), 1, light.specular.v);
	glUniform4fv(glGetUniformLocation(program, "light.position"), 1, light.position.v);

	glUniform3fv(glGetUniformLocation(program, "light.attenuation"), 1, light.attenuation.v);
	glUniform3fv(glGetUniformLocation(program, "light.spotDirection"), 1, light.spotDirection.v);

	glUniform1fv(glGetUniformLocation(program, "light.spotExponent"), 1, &light.spotExponent);
	glUniform1fv(glGetUniformLocation(program, "light.spotCosCutoff"), 1, &light.spotCosCutoff);
}
