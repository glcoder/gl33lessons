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
	Camera() {}
	~Camera() {}

	void setup(const ShaderProgram &program, const RenderObject &object) const
	{
		const GLuint handle = program.getHandle();
		const mat4   model  = object.getModelMatrix();
		const mat3   normal = transpose(mat3(inverse(model)));

		const mat4 view = GL::FromEuler(m_rotation)
			* GL::Translation(-m_position);

		const mat4 viewProjection      = m_projection * view;
		const mat4 modelViewProjection = viewProjection * model;

		glUniformMatrix4fv(glGetUniformLocation(handle, "camera.model"),  1, GL_TRUE, model.m);
		glUniformMatrix3fv(glGetUniformLocation(handle, "camera.normal"), 1, GL_TRUE, normal.m);

		glUniformMatrix4fv(glGetUniformLocation(handle, "camera.viewProjection"), 1, GL_TRUE, viewProjection.m);
		
		glUniformMatrix4fv(glGetUniformLocation(handle, "camera.modelViewProjection"), 1, GL_TRUE, modelViewProjection.m);
		glUniform3fv(glGetUniformLocation(handle, "camera.position"), 1, m_position.v);
	}

protected:
	vec3 m_position;
	vec3 m_rotation;
	mat4 m_projection;
};

#endif /* CAMERA_H */
