#include "Mesh.h"

Mesh::Mesh():
	m_vao(0), m_vbo(0), m_ibo(0), m_vcount(0), m_icount(0)
{
}

Mesh::~Mesh()
{
	destroy();
}

bool Mesh::create(const char *name)
{
	ASSERT(name);
	ASSERT(m_vao == 0);

	return false;
}

void Mesh::createFullscreenQuad()
{
	ASSERT(m_vao == 0);

	static const VertexPT vertices[6] = {
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
	glBufferData(GL_ARRAY_BUFFER, vcount * sizeof(VertexPT),
		vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(VERT_POSITION, 3, GL_FLOAT, GL_FALSE,
            sizeof(VertexPT), GL_OFFSET(0));
    glEnableVertexAttribArray(VERT_POSITION);

    glVertexAttribPointer(VERT_TEXCOORD, 2, GL_FLOAT, GL_FALSE,
            sizeof(VertexPT), GL_OFFSET(sizeof(float3)));
    glEnableVertexAttribArray(VERT_TEXCOORD);
}

void Mesh::destroy()
{
	if (glIsBuffer(m_ibo) == GL_TRUE)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_ibo);
	}

	if (glIsBuffer(m_vbo) == GL_TRUE)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_vbo);
	}

	if (glIsVertexArray(m_vao) == GL_TRUE)
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_vao);
	}

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

