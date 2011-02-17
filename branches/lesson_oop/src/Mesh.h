#ifndef MESH_H
#define MESH_H

#include "common.h"
#include "math/math3d.h"
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

	bool load(const char *name, bool calcTBN = false, float scale = 0.0f);

	void render() const;

protected:
	GLuint   m_vao, m_vbo, m_ibo, m_tbnvbo;
	uint32_t m_vcount, m_icount;
};

#endif /* MESH_H */
