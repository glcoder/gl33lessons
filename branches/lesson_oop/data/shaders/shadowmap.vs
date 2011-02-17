layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;
layout(location = VERT_NORMAL)   in vec3 normal;
layout(location = VERT_TANGENT)  in vec3 tangent;
layout(location = VERT_BINORMAL) in vec3 binormal;

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
	mat3 tbn        = transpose(mat3(tangent, binormal, normal));
	vertex.smcoord  = camera.light * vertexpos;
	vertex.texcoord = texcoord;
	vertex.normal   = camera.normal * normal;
	vertex.lightDir = normalize(tbn * transpose(camera.normal) * light.position.xyz);
	vertex.viewDir  = camera.position - vertexpos.xyz;
	gl_Position     = camera.viewProjection * vertexpos;
}
