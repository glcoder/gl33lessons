layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;
layout(location = VERT_NORMAL)   in vec3 normal;

uniform struct Camera
{
	mat4 model;
	mat4 viewProjection;
	mat4 light;
	mat3 normal;
	vec3 position;
} camera;

uniform struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
} light;

out Vertex
{
	vec2 texcoord;
	vec4 smcoord;
	vec3 lightDir;
	vec3 viewDir;
	vec3 normal;
} vertex;

void main(void)
{
	vec4 vertexpos  = camera.model * vec4(position, 1.0);
	vertex.smcoord  = camera.light * vertexpos;
	vertex.texcoord = texcoord;
	vertex.normal   = camera.normal * normal;
	// if we switch multiply order we don't need to transpose matricies
	vertex.lightDir = normalize(light.position.xyz);
	vertex.viewDir  = camera.position - vertexpos.xyz;
	gl_Position     = camera.viewProjection * vertexpos;
}
