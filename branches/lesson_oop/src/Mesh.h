#define MESH_H
#define MESH_H

#include "common.h"
#include "OpenGL.h"

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

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool create(const char *name);
	void createFullscreenQuad();

	void destroy();

	void render() const;

protected:
	GLuint   m_vao, m_vbo, m_ibo;
	uint32_t m_vcount, m_icount;
};

#endif /* MESH_H */

