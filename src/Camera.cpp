#include "Camera.h"

void CameraCreate(Camera &camera, float x, float y, float z)
{
	camera.position = vec3(x, y, z);
}

void CameraPerspective(Camera &camera, float fov, float aspect, float zNear, float zFar)
{
	camera.projection = GLPerspective(fov, aspect, zNear, zFar);
}

void CameraRotate(Camera &camera, float x, float y, float z)
{
	camera.rotation += vec3(x, y, z);
}

void CameraMove(Camera &camera, float x, float y, float z)
{
	vec3 move = transpose(mat3(GLRotation(camera.rotation))) * vec3(x, y, z);

	camera.position += move;
}

void CameraSetup(GLuint program, const Camera &camera, const mat4 &model)
{
	mat4 modelViewProjection = camera.projection * (GLRotation(camera.rotation)
		* GLTranslation(-camera.position)) * model;

	glUniformMatrix4fv(glGetUniformLocation(program, "transform.model"), 1, GL_TRUE, model.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "transform.modelViewProjection"), 1, GL_TRUE, modelViewProjection.m);
	glUniformMatrix3fv(glGetUniformLocation(program, "transform.normal"), 1, GL_TRUE, mat3(model).m);
	glUniform3fv(glGetUniformLocation(program, "transform.viewPosition"), 1, camera.position.v);
}
