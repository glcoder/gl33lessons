#version 330 core

#define FRAG_OUTPUT0 0

// параметры материала
uniform struct Material
{
	sampler2D texture;
} material;

// параметры полученные из вершинного шейдера
in Vertex {
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

void main(void)
{
	// вычислим итоговый цвет пикселя на экране с учетом текстуры
	color = texture(material.texture, Vert.texcoord);
}
