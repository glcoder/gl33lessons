uniform sampler2D depthTexture;

// параметры полученные из вершинного шейдера
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

void main(void)
{
	// вычислим итоговый цвет пикселя на экране с учетом текстуры
	color = texture(depthTexture, Vert.texcoord);
}
