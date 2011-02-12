#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"
#include "OpenGL.h"
#include "Texture.h"
#include "Shader.h"

class Material
{
public:
	Material();
	~Material();

	void setTexture(const Texture *texture);
	const Texture *getTexure() const;

	void setAmbient(float x, float y, float z, float w);
	void setDiffuse(float x, float y, float z, float w);
	void setSpecular(float x, float y, float z, float w);
	void setEmission(float x, float y, float z, float w);
	void setShininess(float x);

	void setup(const ShaderProgram &program, GLint textureUnit = 0) const;

protected:
	const Texture *m_texture;
	float4 m_ambient;
	float4 m_diffuse;
	float4 m_specular;
	float4 m_emission;
	float  m_shininess;
};

#endif /* MATERIAL_H */
