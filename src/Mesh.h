#ifndef MESH_H
#define MESH_H

#include "common.h"
#include "math/math3d.h"
#include "math/mathgl.h"
#include "OpenGL.h"

struct Mesh
{
	GLuint   vao, vbo[2];
	uint32_t vcount, icount;

	vec3 position;
	mat3 rotation;
};

void MeshCreateQuad(Mesh &mesh, const vec3 &position, float size);
void MeshCreatePlane(Mesh &mesh, const vec3 &position, float size);
void MeshCreateCube(Mesh &mesh, const vec3 &position, float size);
void MeshCreateSphere(Mesh &mesh, const vec3 &position, float size);
void MeshCreateTorus(Mesh &mesh, const vec3 &position, float size);

const mat4 MeshGetModelMatrix(const Mesh &mesh);

void MeshRender(const Mesh &mesh);
void MeshDestroy(Mesh &mesh);

#endif /* MESH_H */
