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

const mat4 RenderObject::getModelMatrix() const
{
	return GL::Translation(m_position) * mat4(m_rotation);
}

void RenderObject::render(const ShaderProgram &program, GLint textureUnit) const
{
	ASSERT(m_material);
	ASSERT(m_mesh);

	//const GLuint handle = program.getHandle();

	m_material->setup(program, textureUnit);
	m_mesh->render();
}
