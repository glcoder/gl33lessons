#ifndef LIGHT_H
#define LIGHT_H

#include "common.h"
#include "OpenGL.h"
#include "Shader.h"

enum LightType { LIGHT_POINT = 0, LIGHT_DIRECTIONAL, LIGHT_SPOT };

class Light
{
public:
	Light(LightType type = LIGHT_POINT);
	~Light();

	void setType(LightType type);
	void setAmbient(float x, float y, float z, float w);
	void setDiffuse(float x, float y, float z, float w);
	void setSpecular(float x, float y, float z, float w);
	void setPosition(float x, float y, float z, float w);
	void setAttenuation(float x, float y, float z);
	void setSpotDirection(float x, float y, float z);
	void setSpotExponent(float x);
	void setSpotCosCutoff(float x);

	void setup(const ShaderProgram &program) const;

protected:
	LightType m_type;
	float4    m_ambient;
	float4    m_diffuse;
	float4    m_specular;
	float4    m_position;
	float3    m_attenuation;
	float3    m_spotDirection;
	float     m_spotExponent;
	float     m_spotCosCutoff;
};

#endif /* LIGHT_H */
