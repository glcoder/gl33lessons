#include "Camera.h"

Camera::Camera():
	m_position(vec3_zero), m_rotation(vec3_zero),
	m_projection(mat4_identity)
{
}

Camera::~Camera()
{
}

/*
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
*/

void Camera::lookAt(const vec3 &position, const vec3 &center, const vec3 &up)
{
	m_rotation = GL::toEuler(GL::lookAt(position, center, up));
	m_position = position;
}

void Camera::perspective(float fov, float aspect, float zNear, float zFar)
{
	m_projection = GL::perspective(fov, aspect, zNear, zFar);
}

void Camera::orthographic(float left, float right, float bottom, float top,
	float zNear, float zFar)
{
	m_projection = GL::orthographic(left, right, bottom, top, zNear, zFar);
}

void Camera::rotate(const vec3 &angles)
{
	m_rotation += angles;
}

void Camera::move(const vec3 &destination)
{
	vec3 move = transpose(mat3(GL::fromEuler(m_rotation))) * destination;
	m_position += move;
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

void Camera::setupLight(const ShaderProgram &program) const
{
	static const mat4 bias(
		0.5f, 0.0f, 0.0f, 0.5f,
		0.0f, 0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	const mat4 view           = GL::fromEuler(m_rotation) * GL::translation(-m_position);
	const mat4 viewProjection = bias * m_projection * view;

	glUniformMatrix4fv(glGetUniformLocation(program.getHandle(), "camera.light"), 1, GL_TRUE, viewProjection.m);
}
