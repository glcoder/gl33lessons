#include "Mesh.h"

struct vertex
{
	float3 position;
	float2 texcoord;
	float3 normal;
};

void MeshCreate(Mesh &mesh, const vec3 &position, const vertex *vertices, uint32_t vcount,
	const uint32_t *indices, uint32_t icount)
{
	ASSERT(vertices);
	ASSERT(indices);

	mesh.vcount = vcount;
	mesh.icount = icount;

	mesh.position = position;
	mesh.rotation = mat3_identity;

	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	glGenBuffers(2, mesh.vbo);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vcount * sizeof(vertex), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, icount * sizeof(uint32_t), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(VERT_POSITION, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (const GLvoid*)0);
	glEnableVertexAttribArray(VERT_POSITION);

	glVertexAttribPointer(VERT_TEXCOORD, 2, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (const GLvoid*)(sizeof(float3)));
	glEnableVertexAttribArray(VERT_TEXCOORD);

	glVertexAttribPointer(VERT_NORMAL, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (const GLvoid*)(sizeof(float3) + sizeof(float2)));
	glEnableVertexAttribArray(VERT_NORMAL);

	OPENGL_CHECK_FOR_ERRORS();
}

void MeshDestroy(Mesh &mesh)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, mesh.vbo);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &mesh.vao);
}

void MeshCreatePlane(Mesh &mesh, const vec3 &position, float size)
{
	#include "models/plane.h"
	return MeshCreate(mesh, position, vertices, vcount, indices, icount);
}

void MeshCreateCube(Mesh &mesh, const vec3 &position, float size)
{
	#include "models/cube.h"
	return MeshCreate(mesh, position, vertices, vcount, indices, icount);
}

void MeshCreateSphere(Mesh &mesh, const vec3 &position, float size)
{
	#include "models/sphere.h"
	return MeshCreate(mesh, position, vertices, vcount, indices, icount);
}

void MeshCreateTorus(Mesh &mesh, const vec3 &position, float size)
{
	#include "models/torus.h"
	return MeshCreate(mesh, position, vertices, vcount, indices, icount);
}

const mat4 MeshGetModelMatrix(const Mesh &mesh)
{
	return GLTranslation(mesh.position) * mat4(mesh.rotation);
}

void MeshRender(const Mesh &mesh)
{
	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, mesh.icount, GL_UNSIGNED_INT, NULL);
}
