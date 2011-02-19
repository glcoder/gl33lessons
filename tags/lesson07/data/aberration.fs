// текстуры
uniform sampler2D colorTexture, depthTexture;

// параметры полученные из вершинного шейдера
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

vec3 filter(in vec2 texcoord)
{
	return vec3(
		textureOffset(colorTexture, texcoord, ivec2(2,2)).r,
		texture(colorTexture, texcoord).g,
		textureOffset(colorTexture, texcoord, ivec2(4,4)).b
	);
}

void main(void)
{
	vec3 texel = Vert.texcoord.x < 0.5 ? filter(Vert.texcoord)
		: texture(colorTexture, Vert.texcoord).rgb;

	color = vec4(texel, 1.0);
}
