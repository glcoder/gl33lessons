#ifndef MESH_H
#define MESH_H

#include "common.h"
#include "VFS.h"
#include "OpenGL.h"
#include "modcon.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	void createFullscreenQuad();
	void destroy();

	bool load(const char *name);

	void render() const;

protected:
	GLuint   m_vao, m_vbo, m_ibo;
	uint32_t m_vcount, m_icount;
};

#endif /* MESH_H */
