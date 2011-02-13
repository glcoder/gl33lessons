#include "Camera.h"

Camera::Camera():
	m_position(vec3_zero), m_rotation(vec3_zero),
	m_projection(mat4_identity)
{
}

Camera::~Camera()
{
}

void Camera::setPosition(const vec3 &position)
{
	m_position = position;
}

void Camera::setRotation(const vec3 &rotation)
{
	m_rotation = rotation;
}

void Camera::setProjection(const mat4 &projection)
{
	m_projection = projection;
}

void Camera::setup(const ShaderProgram &program, const RenderObject &object) const
{
	const GLuint handle = program.getHandle();
	const mat4   model  = object.getModelMatrix();
	const mat3   normal = transpose(mat3(inverse(model)));

	const mat4 view = GL::fromEuler(m_rotation)
		* GL::translation(-m_position);

	const mat4 viewProjection      = m_projection * view;
	const mat4 modelViewProjection = viewProjection * model;

	glUniformMatrix4fv(glGetUniformLocation(handle, "camera.model"),  1, GL_TRUE, model.m);
	glUniformMatrix3fv(glGetUniformLocation(handle, "camera.normal"), 1, GL_TRUE, normal.m);

	glUniformMatrix4fv(glGetUniformLocation(handle, "camera.viewProjection"), 1, GL_TRUE, viewProjection.m);

	glUniformMatrix4fv(glGetUniformLocation(handle, "camera.modelViewProjection"), 1, GL_TRUE, modelViewProjection.m);
	glUniform3fv(glGetUniformLocation(handle, "camera.position"), 1, m_position.v);
}
