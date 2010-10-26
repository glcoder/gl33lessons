#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"
#include "math/math3d.h"
#include "math/mathgl.h"
#include "OpenGL.h"
#include "Shader.h"

struct Camera
{
	vec3 position;
	vec3 rotation;

	mat4 projection;
};

void CameraCreate(Camera &camera, float x, float y, float z);
void CameraPerspective(Camera &camera, float fov, float aspect, float zNear, float zFar);
void CameraOrtho(Camera &camera, float left, float right,
	float bottom, float top, float zNear, float zFar);
void CameraRotate(Camera &camera, float x, float y, float z);
void CameraMove(Camera &camera, float x, float y, float z);
void CameraSetup(GLuint program, const Camera &camera, const mat4 &model);
void CameraSetupLight(GLuint program, const Camera &camera);

#endif /* CAMERA_H */
