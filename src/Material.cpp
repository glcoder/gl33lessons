#include "Material.h"

Material::Material():
	m_texture(NULL), m_textureNormal(NULL)
{
	float4_set(m_ambient, 0.2f, 0.2f, 0.2f, 1.0f);
	float4_set(m_diffuse, 0.8f, 0.8f, 0.8f, 1.0f);
	float4_set(m_specular, 0.0f, 0.0f, 0.0f, 1.0f);
	float4_set(m_emission, 0.0f, 0.0f, 0.0f, 1.0f);
	m_shininess = 0.0f;
}

Material::~Material()
{
}

void Material::setTexture(const Texture *texture)
{
	m_texture = texture;
}

const Texture *Material::getTexure() const
{
	return m_texture;
}

void Material::setTextureNormal(const Texture *texture)
{
	m_textureNormal = texture;
}

const Texture *Material::getTexureNormal() const
{
	return m_textureNormal;
}

void Material::setAmbient(float x, float y, float z, float w)
{
	float4_set(m_ambient, x, y, z, w);
}

void Material::setDiffuse(float x, float y, float z, float w)
{
	float4_set(m_diffuse, x, y, z, w);
}

void Material::setSpecular(float x, float y, float z, float w)
{
	float4_set(m_specular, x, y, z, w);
}

void Material::setEmission(float x, float y, float z, float w)
{
	float4_set(m_emission, x, y, z, w);
}

void Material::setShininess(float x)
{
	m_shininess = x;
}

void Material::setup(const ShaderProgram &program, GLint textureUnit, GLint textureNormalUnit) const
{
	const GLuint handle = program.getHandle();

	if (m_texture)
		m_texture->setup(program, "material.texture", textureUnit, false);

	if (m_textureNormal)
		m_textureNormal->setup(program, "material.normal", textureNormalUnit, false);

	glUniform4fv(glGetUniformLocation(handle, "material.ambient"), 1, m_ambient);
	glUniform4fv(glGetUniformLocation(handle, "material.diffuse"), 1, m_diffuse);
	glUniform4fv(glGetUniformLocation(handle, "material.specular"), 1, m_specular);
	glUniform4fv(glGetUniformLocation(handle, "material.emission"), 1, m_emission);
	glUniform1fv(glGetUniformLocation(handle, "material.shininess"), 1, &m_shininess);
}
