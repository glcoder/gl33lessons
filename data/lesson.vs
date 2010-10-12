uniform mat4 modelViewProjectionMatrix;
uniform mat3 modelMatrix;
uniform mat3 normalMatrix;

layout(location = POSITION_LOCATION)  in vec3 position;
layout(location = TEXCOORD_LOCATION)  in vec2 texcoord;
layout(location = NORMAL_LOCATION)    in vec3 normal;
layout(location = OFFSET_LOCATION)    in vec3 offset;
layout(location = DIRECTION_LOCATION) in vec3 direction;

out Vertex {
	vec3 color;
	vec2 texcoord;
} Vert;

mat3 orient(vec3 from, vec3 to)
{
	vec3 f = normalize(from - to);
	vec3 s = normalize(cross(vec3(0.0, 1.0, 0.0), f));
	vec3 u = cross(f, s);

	// form row-major matrix
	return mat3(s.x, u.x, f.x,
	            s.y, u.y, f.y,
	            s.z, u.z, f.z);
}

void main(void)
{
	mat3 rotate   = orient(position, position + direction);
	vec3 vertex   = rotate * position + offset;
	vec3 vertexN  = normalize(normalMatrix * rotate * normal);
	float NdotL   = clamp(dot(vertexN, modelMatrix * vertex), 0.2, 1.0);
	gl_Position   = modelViewProjectionMatrix * vec4(vertex, 1.0);
	Vert.color    = normalize(direction + vec3(1.0, 1.0, 1.0)) * NdotL;
	Vert.texcoord = texcoord;
}
