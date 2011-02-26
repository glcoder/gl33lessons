#include "RenderObject.h"

RenderObject::RenderObject():
	m_material(NULL), m_mesh(NULL),
	m_position(vec3_zero), m_rotation(mat3_identity)
{
}

RenderObject::~RenderObject()
{
}

void RenderObject::setMaterial(const Material *material)
{
	m_material = material;
}

const Material *RenderObject::getMaterial() const
{
	return m_material;
}

void RenderObject::setMesh(const Mesh *mesh)
{
	m_mesh = mesh;
}

const Mesh *RenderObject::getMesh() const
{
	return m_mesh;
}

void RenderObject::setPosition(const vec3 &position)
{
	m_position = position;
}

void RenderObject::setRotation(const mat3 &rotation)
{
	m_rotation = rotation;
}

vec3 RenderObject::getPosition() const
{
	return m_position;
}

mat3 RenderObject::getRotation() const
{
	return m_rotation;
}

const mat4 RenderObject::getModelMatrix() const
{
	return GL::translation(m_position) * mat4(m_rotation);
}

void RenderObject::render(const ShaderProgram &program, GLint textureUnit,
	GLint textureNormalUnit) const
{
	ASSERT(m_material);
	ASSERT(m_mesh);

	m_material->setup(program, textureUnit, textureNormalUnit);
	m_mesh->render();
}
