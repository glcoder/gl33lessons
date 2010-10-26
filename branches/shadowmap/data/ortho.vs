#version 330 core

#define VERT_POSITION 0
#define VERT_TEXCOORD 1

layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;

// параметры преобразований
uniform struct Transform
{
	mat4 modelViewProjection;
} transform;

// параметры для фрагментного шейдера
out Vertex {
	vec2 texcoord;
} Vert;

void main(void)
{
	// передаем текстурные координаты
	Vert.texcoord = texcoord;

	// переводим координаты вершины в однородные
	gl_Position = transform.modelViewProjection * vec4(position, 1.0);
}
