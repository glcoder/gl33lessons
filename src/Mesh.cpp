#include "Mesh.h"

//#pragma pack(1)
struct VertexPT
{
	float3 position;
	float2 texcoord;
};

struct VertexPTN
{
	float3 position;
	float2 texcoord;
	float3 normal;
};
//#pragma pack()

Mesh::Mesh():
	m_vao(0), m_vbo(0), m_ibo(0), m_vcount(0), m_icount(0)
{
}

Mesh::~Mesh()
{
	destroy();
}

void Mesh::createFullscreenQuad()
{
	ASSERT(m_vao == 0);

	static const VertexPT vertices[6] = {
		{{-1.0f,-1.0f, 0.0f}, {0.0f,1.0f}},
		{{ 1.0f,-1.0f, 0.0f}, {1.0f,1.0f}},
		{{-1.0f, 1.0f, 0.0f}, {0.0f,0.0f}},
		{{ 1.0f,-1.0f, 0.0f}, {1.0f,1.0f}},
		{{ 1.0f, 1.0f, 0.0f}, {1.0f,0.0f}},
		{{-1.0f, 1.0f, 0.0f}, {0.0f,0.0f}}
	};

	m_vcount = 6;
	m_icount = 0;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vcount * sizeof(VertexPT),
		GL_PVOID(vertices), GL_STATIC_DRAW);

	glVertexAttribPointer(VERT_POSITION, 3, GL_FLOAT, GL_FALSE,
			sizeof(VertexPT), GL_PVOID(0));
	glEnableVertexAttribArray(VERT_POSITION);

	glVertexAttribPointer(VERT_TEXCOORD, 2, GL_FLOAT, GL_FALSE,
			sizeof(VertexPT), GL_PVOID(sizeof(float3)));
	glEnableVertexAttribArray(VERT_TEXCOORD);
}

void Mesh::destroy()
{
	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);

	m_vao = m_vbo = m_ibo = 0;
	m_vcount = m_icount = 0;
}

void Mesh::render() const
{
	ASSERT(m_vao);

	glBindVertexArray(m_vao);

	if (m_icount)
		glDrawElements(GL_TRIANGLES, m_icount, GL_UNSIGNED_INT, NULL);
	else
		glDrawArrays(GL_TRIANGLES, 0, m_vcount);
}

bool Mesh::load(const char *name, float scale)
{
	ASSERT(name);
	ASSERT(m_vao == 0);

	uint8_t     *buffer, *pbuffer;
	uint32_t    size;
	mesh_head_t *head;

	if (!VFS::load(name, VFS_BINARY, &buffer, &size))
		return false;

	if (size <= sizeof(mesh_head_t))
	{
		LOG_ERROR("Too small file '%s'\n", name);
		delete[] buffer;
		return false;
	}

	head = (mesh_head_t *)buffer;

	if ((size - sizeof(mesh_head_t)) != (sizeof(mesh_vertex_t) * head->vcount
		+ sizeof(mesh_index_t) * head->icount))
	{
		LOG_ERROR("Corrupted mesh in file '%s'\n", name);
		delete[] buffer;
		return false;
	}

	m_vcount = head->vcount;
	m_icount = head->icount;

	pbuffer = buffer + sizeof(mesh_head_t);

	if (scale != 0.0f)
	{
		mesh_vertex_t *vertices = (mesh_vertex_t *)pbuffer;

		for (uint32_t i = 0; i < m_vcount; ++i)
		{
			vertices[i].position[0] *= scale;
			vertices[i].position[1] *= scale;
			vertices[i].position[2] *= scale;
		}
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vcount * sizeof(mesh_vertex_t),
		GL_PVOID(pbuffer), GL_STATIC_DRAW);

	glVertexAttribPointer(VERT_POSITION, 3, GL_FLOAT, GL_FALSE,
			sizeof(mesh_vertex_t), GL_PVOID(0));
	glEnableVertexAttribArray(VERT_POSITION);

	glVertexAttribPointer(VERT_TEXCOORD, 2, GL_FLOAT, GL_FALSE,
			sizeof(mesh_vertex_t), GL_PVOID(sizeof(float3)));
	glEnableVertexAttribArray(VERT_TEXCOORD);

	glVertexAttribPointer(VERT_NORMAL, 3, GL_FLOAT, GL_FALSE,
			sizeof(mesh_vertex_t), GL_PVOID(sizeof(float3) + sizeof(float2)));
	glEnableVertexAttribArray(VERT_NORMAL);

	if (!m_icount)
	{
		delete[] buffer;
		return true;
	}

	pbuffer += m_vcount * sizeof(mesh_vertex_t);

	glGenBuffers(1, &m_ibo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_icount * sizeof(mesh_index_t),
		GL_PVOID(pbuffer), GL_STATIC_DRAW);

	delete[] buffer;
	return true;
}
