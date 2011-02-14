uniform struct Material
{
	sampler2D texture;
} material;

in Vertex
{
	vec2 texcoord;
} vertex;

layout(location = FRAG_OUTPUT0) out vec4 color;

void main(void)
{
	color = texture(material.texture, vertex.texcoord);
}
