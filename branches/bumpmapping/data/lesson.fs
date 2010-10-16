uniform sampler2D colorTexture;
uniform sampler2D normalTexture;

in Vertex {
	vec2 texcoord;
	vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

void main(void)
{
	vec3  bumpNormal = normalize((texture(normalTexture, Vert.texcoord).rgb - 0.5) * 2.0);
	vec3  lightDir   = -normalize(Vert.lightDir);
	float bumpDot    = max(dot(bumpNormal, lightDir), 0.0);
	vec3  lightRefl  = normalize(bumpNormal * bumpDot * 2.0 - lightDir);
	vec3  viewDir    = -normalize(Vert.viewDir);
	float reflDot    = max(dot(lightRefl, viewDir), 0.0);
	vec3  specular   = vec3(0.2) * pow(reflDot, 15.0);
	color            = vec4(texture(colorTexture, Vert.texcoord).rgb * bumpDot + specular, 1.0);
}
