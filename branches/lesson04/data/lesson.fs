#version 330 core

#define FRAG_OUTPUT0 0

uniform struct PointLight
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 attenuation;
} light;

uniform struct Material
{
	sampler2D texture;

	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  emission;
	float shininess;
} material;

in Vertex {
	vec2  texcoord;
	vec3  normal;
	vec3  lightDir;
	vec3  viewDir;
	float distance;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

void main(void)
{
	// нормализуем полученные данные для коррекции интерполяции
	vec3 normal   = normalize(Vert.normal);
	vec3 lightDir = normalize(Vert.lightDir);
	vec3 viewDir  = normalize(Vert.viewDir);

	// цвет текселя
	vec4 textureColor = texture(material.texture, Vert.texcoord);

	// коэффициент затухания
	float attenuation = 1.0 / (light.attenuation[0] +
		light.attenuation[1] * Vert.distance +
		light.attenuation[2] * Vert.distance * Vert.distance);

	// свечение материала
	color = material.emission;

	// добавим фоновое освещение
	color += textureColor * material.ambient * light.ambient * attenuation;

	// добавим рассеянный свет
	float NdotL = max(dot(normal, normalize(lightDir)), 0.0);
	color += textureColor * material.diffuse * light.diffuse * NdotL * attenuation;

	// добавим отраженный свет
	float RdotV = pow(max(dot(reflect(-lightDir, normal), viewDir), 0.0), material.shininess);
	color += textureColor * material.specular * light.specular * RdotV * attenuation;
}
