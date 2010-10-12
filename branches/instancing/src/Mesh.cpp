#include "Mesh.h"

struct vertex
{
	float position[3];
	float texcoord[2];
	float normal[3];
};

Mesh *MeshCreateCube(float size)
{
	const uint32_t vcount = 24;
	const uint32_t icount = 36;

	// вершины куба
	const vertex vertices[vcount] = {
		// front
		{{-size, size, size}, {0.0f,1.0f}, { 0.0f, 0.0f, 1.0f}},
		{{ size, size, size}, {1.0f,1.0f}, { 0.0f, 0.0f, 1.0f}},
		{{ size,-size, size}, {1.0f,0.0f}, { 0.0f, 0.0f, 1.0f}},
		{{-size,-size, size}, {0.0f,0.0f}, { 0.0f, 0.0f, 1.0f}},
		// back
		{{ size, size,-size}, {0.0f,1.0f}, { 0.0f, 0.0f,-1.0f}},
		{{-size, size,-size}, {1.0f,1.0f}, { 0.0f, 0.0f,-1.0f}},
		{{-size,-size,-size}, {1.0f,0.0f}, { 0.0f, 0.0f,-1.0f}},
		{{ size,-size,-size}, {0.0f,0.0f}, { 0.0f, 0.0f,-1.0f}},
		// top
		{{-size, size,-size}, {0.0f,1.0f}, { 0.0f, 1.0f, 0.0f}},
		{{ size, size,-size}, {1.0f,1.0f}, { 0.0f, 1.0f, 0.0f}},
		{{ size, size, size}, {1.0f,0.0f}, { 0.0f, 1.0f, 0.0f}},
		{{-size, size, size}, {0.0f,0.0f}, { 0.0f, 1.0f, 0.0f}},
		// bottom
		{{ size,-size,-size}, {0.0f,1.0f}, { 0.0f,-1.0f, 0.0f}},
		{{-size,-size,-size}, {1.0f,1.0f}, { 0.0f,-1.0f, 0.0f}},
		{{-size,-size, size}, {1.0f,0.0f}, { 0.0f,-1.0f, 0.0f}},
		{{ size,-size, size}, {0.0f,0.0f}, { 0.0f,-1.0f, 0.0f}},
		// left
		{{-size, size,-size}, {0.0f,1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-size, size, size}, {1.0f,1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-size,-size, size}, {1.0f,0.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-size,-size,-size}, {0.0f,0.0f}, {-1.0f, 0.0f, 0.0f}},
		// right
		{{ size, size, size}, {0.0f,1.0f}, { 1.0f, 0.0f, 0.0f}},
		{{ size, size,-size}, {1.0f,1.0f}, { 1.0f, 0.0f, 0.0f}},
		{{ size,-size,-size}, {1.0f,0.0f}, { 1.0f, 0.0f, 0.0f}},
		{{ size,-size, size}, {0.0f,0.0f}, { 1.0f, 0.0f, 0.0f}}
	};

	const uint32_t indices[icount] = {
		 0, 3, 1,  1, 3, 2, // front
		 4, 7, 5,  5, 7, 6, // back
		 8,11, 9,  9,11,10, // top
		12,15,13, 13,15,14, // bottom
		16,19,17, 17,19,18, // left
		20,23,21, 21,23,22  // right
	};

	Mesh *mesh = new Mesh;
	ASSERT(mesh);

	mesh->vcount = vcount;
	mesh->icount = icount;

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	glGenBuffers(2, mesh->vbo);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vcount * sizeof(vertex), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, icount * sizeof(uint32_t), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (const GLvoid*)0);
	glVertexAttribDivisor(POSITION_LOCATION, 0);
	glEnableVertexAttribArray(POSITION_LOCATION);

	glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (const GLvoid*)(3 * sizeof(float)));
	glVertexAttribDivisor(TEXCOORD_LOCATION, 0);
	glEnableVertexAttribArray(TEXCOORD_LOCATION);

	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (const GLvoid*)(5 * sizeof(float)));
	glVertexAttribDivisor(NORMAL_LOCATION, 0);
	glEnableVertexAttribArray(NORMAL_LOCATION);

	OPENGL_CHECK_FOR_ERRORS();

	return mesh;
}

void MeshRender(const Mesh *mesh)
{
	ASSERT(mesh);

	glBindVertexArray(mesh->vao);
	glDrawElements(GL_TRIANGLES, mesh->icount, GL_UNSIGNED_INT, NULL);
}

void MeshDestroy(Mesh *mesh)
{
	ASSERT(mesh);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, mesh->vbo);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &mesh->vao);

	delete mesh;
}
