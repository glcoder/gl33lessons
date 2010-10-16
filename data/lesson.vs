uniform mat4 modelViewProjectionMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;

layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;
layout(location = VERT_NORMAL)   in vec3 normal;
layout(location = VERT_TANGENT)  in vec3 tangent;
layout(location = VERT_BINORMAL) in vec3 binormal;

out Vertex {
	vec2 texcoord;
	vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
} Vert;

void main(void)
{
	vec4 vertex    = modelViewMatrix * vec4(position, 1.0);
	mat3 tbnMatrix = transpose(mat3(tangent, binormal, normal));
	Vert.texcoord  = texcoord;
	Vert.normal    = normalize(normalMatrix * normal);
	Vert.lightDir  = inverse(normalMatrix) * vec3(0.0, 0.0, -1.0);
	Vert.lightDir  = normalize(tbnMatrix * Vert.lightDir);
	Vert.viewDir   = vec3(0.0, 0.0, -1.0);

	gl_Position    = modelViewProjectionMatrix * vec4(position, 1.0);
}
