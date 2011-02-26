#include "Mesh.h"

struct quad_vertex_t
{
	float3 position;
	float2 texcoord;
};

struct bn_vertex_t
{
	float3 tangent;
	float3 binormal;
};

struct mesh_vertex_t
{
	float3 position;
	float3 normal;
	float2 texcoord;
};

struct mesh_head_t
{
	uint32_t vcount;
	uint32_t icount;
};

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

	static const quad_vertex_t vertices[6] = {
		{{-1.0f,-1.0f, 0.0f}, {0.0f,0.0f}},
		{{ 1.0f,-1.0f, 0.0f}, {1.0f,0.0f}},
		{{-1.0f, 1.0f, 0.0f}, {0.0f,1.0f}},
		{{ 1.0f,-1.0f, 0.0f}, {1.0f,0.0f}},
		{{ 1.0f, 1.0f, 0.0f}, {1.0f,1.0f}},
		{{-1.0f, 1.0f, 0.0f}, {0.0f,1.0f}}
	};

	m_vcount = 6;
	m_icount = 0;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vcount * sizeof(quad_vertex_t),
		GL_PVOID(vertices), GL_STATIC_DRAW);

	glVertexAttribPointer(VERT_POSITION, 3, GL_FLOAT, GL_FALSE,
			sizeof(quad_vertex_t), GL_PVOID(0));
	glEnableVertexAttribArray(VERT_POSITION);

	glVertexAttribPointer(VERT_TEXCOORD, 2, GL_FLOAT, GL_FALSE,
			sizeof(quad_vertex_t), GL_PVOID(sizeof(float3)));
	glEnableVertexAttribArray(VERT_TEXCOORD);
}

void Mesh::destroy()
{
	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_tbnvbo);
	glDeleteVertexArrays(1, &m_vao);

	m_vao = m_vbo = m_ibo = m_tbnvbo = 0;
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

static void CalcTBN(const mesh_vertex_t *vertices, uint32_t vcount,
	const uint32_t *indices, uint32_t icount, bn_vertex_t *tb)
{
	ASSERT(vertices);
	ASSERT(indices);
	ASSERT(tb);

	// copypaste http://www.terathon.com/code/tangent.html

	vec3 *tan1 = new vec3[vcount * 2];
	vec3 *tan2 = tan1 + vcount;

	memset(tan1, 0, vcount * 2 * sizeof(vec3));

	for (uint32_t i = 0; i < icount; i += 3)
	{
		const uint32_t i1 = indices[i],
		               i2 = indices[i + 1],
		               i3 = indices[i + 2];

		const vec3 v1(vertices[i1].position),
		           v2(vertices[i2].position),
		           v3(vertices[i3].position);

		const vec2 w1(vertices[i1].texcoord),
		           w2(vertices[i2].texcoord),
		           w3(vertices[i3].texcoord);

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0f / (s1 * t2 - s2 * t1);
		const vec3 sdir = vec3((t2 * x1 - t1 * x2),
			(t2 * y1 - t1 * y2), (t2 * z1 - t1 * z2)) * r;
		const vec3 tdir = vec3((s1 * x2 - s2 * x1),
			(s1 * y2 - s2 * y1), (s1 * z2 - s2 * z1)) * r;

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (uint32_t i = 0; i < vcount; ++i)
	{
		const vec3 n(vertices[i].normal);
		const vec3 p(tan1[i]);
		const vec3 t = normalize(p - n * dot(n, p))
			* (dot(cross(n, p), tan2[i]) < 0.0f ? -1.0f : 1.0f);
		const vec3 b = normalize(cross(n, t));

		float3_set(tb[i].tangent,  t.x, t.y, t.z);
		float3_set(tb[i].binormal, b.x, b.y, b.z);
	}

	delete[] tan1;
}

bool Mesh::load(const char *name, bool calcTBN, vec3 scale)
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

	ASSERT(head->vcount && head->icount);

	if ((size - sizeof(mesh_head_t)) != (sizeof(mesh_vertex_t) * head->vcount
		+ sizeof(uint32_t) * head->icount))
	{
		LOG_ERROR("Corrupted mesh in file '%s'\n", name);
		delete[] buffer;
		return false;
	}

	m_vcount = head->vcount;
	m_icount = head->icount;

	pbuffer = buffer + sizeof(mesh_head_t);

	if (length(scale) != 0.0f)
	{
		mesh_vertex_t *vertices = (mesh_vertex_t *)pbuffer;

		for (uint32_t i = 0; i < m_vcount; ++i)
		{
			vertices[i].position[0] *= scale.x;
			vertices[i].position[1] *= scale.y;
			vertices[i].position[2] *= scale.z;
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

	glVertexAttribPointer(VERT_NORMAL, 3, GL_FLOAT, GL_FALSE,
			sizeof(mesh_vertex_t), GL_PVOID(sizeof(float3)));
	glEnableVertexAttribArray(VERT_NORMAL);

	glVertexAttribPointer(VERT_TEXCOORD, 2, GL_FLOAT, GL_FALSE,
			sizeof(mesh_vertex_t), GL_PVOID(sizeof(float3) * 2));
	glEnableVertexAttribArray(VERT_TEXCOORD);

	if (calcTBN)
	{
		const mesh_vertex_t *vertices = (const mesh_vertex_t *)(pbuffer);
		const uint32_t      *indices  = (const uint32_t *)(pbuffer + m_vcount * sizeof(mesh_vertex_t));

		bn_vertex_t *tb = new bn_vertex_t[m_vcount];

		CalcTBN(vertices, m_vcount, indices, m_icount, tb);

		glGenBuffers(1, &m_tbnvbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_tbnvbo);
		glBufferData(GL_ARRAY_BUFFER, m_vcount * sizeof(bn_vertex_t),
			GL_PVOID(tb), GL_STATIC_DRAW);

		glVertexAttribPointer(VERT_TANGENT, 3, GL_FLOAT, GL_FALSE,
				sizeof(bn_vertex_t), GL_PVOID(0));
		glEnableVertexAttribArray(VERT_TANGENT);

		glVertexAttribPointer(VERT_BINORMAL, 3, GL_FLOAT, GL_FALSE,
				sizeof(bn_vertex_t), GL_PVOID(sizeof(float3)));
		glEnableVertexAttribArray(VERT_BINORMAL);

		delete[] tb;
	}

	pbuffer += m_vcount * sizeof(mesh_vertex_t);

	glGenBuffers(1, &m_ibo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_icount * sizeof(uint32_t),
		GL_PVOID(pbuffer), GL_STATIC_DRAW);

	delete[] buffer;
	return true;
}
