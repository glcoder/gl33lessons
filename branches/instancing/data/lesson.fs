uniform sampler2D colorTexture;

in Vertex {
	vec3 color;
	vec2 texcoord;
} Vert;

layout(location = 0, index = 0) out vec4 color;

void main(void)
{
	color = vec4(texture(colorTexture, Vert.texcoord).rgb * Vert.color, 1.0);
}
