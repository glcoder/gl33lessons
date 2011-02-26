#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"
#include "math/math3d.h"
#include "math/mathgl.h"
#include "OpenGL.h"
#include "Shader.h"
#include "RenderObject.h"

class Camera
{
public:
	Camera();
	~Camera();

	/*
	void setPosition(const vec3 &position);
	void setRotation(const vec3 &rotation);
	void setProjection(const mat4 &projection);
	*/

	void lookAt(const vec3 &position, const vec3 &center, const vec3 &up);

	void perspective(float fov, float aspect, float zNear, float zFar);
	void orthographic(float left, float right, float bottom, float top,
		float zNear, float zFar);

	void rotate(const vec3 &angles);
	void move(const vec3 &destination);

	void setup(const ShaderProgram &program, const RenderObject &object) const;

	void setupLight(const ShaderProgram &program) const;

protected:
	vec3 m_position;
	vec3 m_rotation;
	mat4 m_projection;
};

#endif /* CAMERA_H */
