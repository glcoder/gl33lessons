#include "Light.h"

Light::Light(LightType type)
{
	m_type = type;

	float4_set(m_ambient, 0.0f, 0.0f, 0.0f, 1.0f);
	float4_set(m_diffuse, 1.0f, 1.0f, 1.0f, 1.0f);
	float4_set(m_specular, 1.0f, 1.0f, 1.0f, 1.0f);
	float4_set(m_position, 0.0f, 0.0f, 1.0f, 0.0f);
	float3_set(m_attenuation, 1.0f, 0.0f, 0.0f);
	float3_set(m_spotDirection, 0.0f, 0.0f, -1.0f);
	m_spotExponent  = 0.0f;
	m_spotCosCutoff = -1.0f;
}

Light::~Light()
{
}

void Light::setType(LightType type)
{
	m_type = type;
}

void Light::setAmbient(float x, float y, float z, float w)
{
	float4_set(m_ambient, x, y, z, w);
}

void Light::setDiffuse(float x, float y, float z, float w)
{
	float4_set(m_diffuse, x, y, z, w);
}

void Light::setSpecular(float x, float y, float z, float w)
{
	float4_set(m_specular, x, y, z, w);
}

void Light::setPosition(float x, float y, float z, float w)
{
	float4_set(m_position, x, y, z, w);
}

void Light::setAttenuation(float x, float y, float z)
{
	float3_set(m_attenuation, x, y, z);
}

void Light::setSpotDirection(float x, float y, float z)
{
	float3_set(m_spotDirection, x, y, z);
}

void Light::setSpotExponent(float x)
{
	m_spotExponent = x;
}

void Light::setSpotCosCutoff(float x)
{
	m_spotCosCutoff = x;
}

void Light::setup(const ShaderProgram &program) const
{
	const GLuint handle = program.getHandle();

	glUniform4fv(glGetUniformLocation(handle, "light.ambient"), 1, m_ambient);
	glUniform4fv(glGetUniformLocation(handle, "light.diffuse"), 1, m_diffuse);
	glUniform4fv(glGetUniformLocation(handle, "light.specular"), 1, m_specular);
	glUniform4fv(glGetUniformLocation(handle, "light.position"), 1, m_position);

	glUniform3fv(glGetUniformLocation(handle, "light.attenuation"), 1, m_attenuation);
	glUniform3fv(glGetUniformLocation(handle, "light.spotDirection"), 1, m_spotDirection);

	glUniform1fv(glGetUniformLocation(handle, "light.spotExponent"), 1, &m_spotExponent);
	glUniform1fv(glGetUniformLocation(handle, "light.spotCosCutoff"), 1, &m_spotCosCutoff);
}
