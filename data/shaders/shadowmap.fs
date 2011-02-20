uniform sampler2DShadow depthTexture;

uniform struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
} light;

uniform struct Material
{
	sampler2D texture;
	sampler2D textureNormal;

	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  emission;
	float shininess;
} material;

in Vertex
{
	vec2 texcoord;
	vec4 smcoord;
	vec3 lightDir;
	vec3 viewDir;
	vec3 normal;
} vertex;

layout(location = FRAG_COLOR)  out vec4 color;
layout(location = FRAG_NORMAL) out vec3 normal;

float PCF(in vec4 smcoord)
{
	float res = 0.0;

	res += textureProjOffset(depthTexture, smcoord, ivec2(-1,-1));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 0,-1));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 1,-1));
	res += textureProjOffset(depthTexture, smcoord, ivec2(-1, 0));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 0, 0));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 1, 0));
	res += textureProjOffset(depthTexture, smcoord, ivec2(-1, 1));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 0, 1));
	res += textureProjOffset(depthTexture, smcoord, ivec2( 1, 1));

	return (res / 9.0);
}

void main(void)
{
	//vec3  normal = normalize(vertex.normal);
	vec3  tnormal  = normalize((texture(material.textureNormal, vertex.texcoord).xyz - 0.5) * 2.0);
	vec3  lightDir = normalize(vertex.lightDir);
	vec3  viewDir  = normalize(vertex.viewDir);
	float shadow   = PCF(vertex.smcoord);
	float NdotL    = max(dot(tnormal, lightDir), 0.0);
	float RdotVpow = max(pow(dot(reflect(-lightDir, tnormal), viewDir), material.shininess), 0.0);

	color = (material.emission + material.ambient * light.ambient
		+ material.diffuse * light.diffuse * NdotL
		+ material.specular * light.specular * RdotVpow) * shadow;

	color  = texture(material.texture, vertex.texcoord) * vec4(max(color.xyz, 0.15), 1.0);
	normal = vertex.normal * 0.5 + vec3(0.5);
}
