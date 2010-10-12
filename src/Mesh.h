#ifndef MESH_H
#define MESH_H

#include "common.h"
#include "math3d/math3d.h"
#include "OpenGL.h"
#include "Shader.h"

struct Mesh
{
	GLuint   vao, vbo[2];
	uint32_t vcount, icount;
};

Mesh *MeshCreateCube(float size);
void MeshRender(const Mesh *mesh);
void MeshDestroy(Mesh *mesh);

#endif /* MESH_H */
