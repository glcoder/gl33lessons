#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "common.h"
#include "math/math3d.h"
#include "math/mathgl.h"
#include "OpenGL.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

class RenderObject
{
public:
	RenderObject();
	~RenderObject();

	void setMaterial(const Material *material);
	const Material *getMaterial() const;

	void setMesh(const Mesh *mesh);
	const Mesh *getMesh() const;

	void setPosition(const vec3 &position);
	void setRotation(const mat3 &rotation);

	const mat4 getModelMatrix() const;

	void render(const ShaderProgram &program, GLint textureUnit = 0) const;

protected:
	const Material *m_material;
	const Mesh     *m_mesh;

	vec3 m_position;
	mat3 m_rotation;
};

#endif /* RENDEROBJECT_H */
