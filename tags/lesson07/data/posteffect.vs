layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;

// параметры для фрагментного шейдера
out Vertex
{
	vec2 texcoord;
} Vert;

void main(void)
{
	// передаем текстурные координаты
	Vert.texcoord = texcoord;

	// переводим координаты вершины в однородные
	gl_Position = vec4(position, 1.0);
}
