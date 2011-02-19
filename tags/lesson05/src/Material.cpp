#include "Material.h"

void MaterialDefault(Material &material)
{
	material.texture = 0;

	material.ambient.set(0.2f, 0.2f, 0.2f, 1.0f);
	material.diffuse.set(0.8f, 0.8f, 0.8f, 1.0f);
	material.specular.set(0.0f, 0.0f, 0.0f, 1.0f);
	material.emission.set(0.0f, 0.0f, 0.0f, 1.0f);
	material.shininess = 0.0f;
}

void MaterialSetup(GLuint program, const Material &material)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material.texture);
	glUniform1i(glGetUniformLocation(program, "material.texture"), 0);

	glUniform4fv(glGetUniformLocation(program, "material.ambient"), 1, material.ambient.v);
	glUniform4fv(glGetUniformLocation(program, "material.diffuse"), 1, material.diffuse.v);
	glUniform4fv(glGetUniformLocation(program, "material.specular"), 1, material.specular.v);
	glUniform4fv(glGetUniformLocation(program, "material.emission"), 1, material.emission.v);

	glUniform1fv(glGetUniformLocation(program, "material.shininess"), 1, &material.shininess);
}
