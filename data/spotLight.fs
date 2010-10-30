// параметры источника освещения
uniform struct Light
{
	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  position;
	vec3  attenuation;
	vec3  spotDirection;
	float spotExponent;
	float spotCosCutoff;
} light;

// параметры материала
uniform struct Material
{
	sampler2D texture;

	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  emission;
	float shininess;
} material;

// параметры полученные из вершинного шейдера
in Vertex
{
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

	// расчет угла отклонения от направления прожектора до текущей точки
	float spotEffect = dot(normalize(light.spotDirection), -lightDir);
	// ограничим зону влияние прожектора
	float spot       = float(spotEffect > light.spotCosCutoff);

	// экспоненциальное затухание к краям зоны влияния
	spotEffect = max(pow(spotEffect, light.spotExponent), 0.0);

	// коэффициент затухания прожектора
	float attenuation = spot * spotEffect / (light.attenuation[0] +
		light.attenuation[1] * Vert.distance +
		light.attenuation[2] * Vert.distance * Vert.distance);

	// добавим собственное свечение материала
	color = material.emission;

	// добавим фоновое освещение
	color += material.ambient * light.ambient;

	// добавим рассеянный свет
	float NdotL = max(dot(normal, lightDir), 0.0);
	color += material.diffuse * light.diffuse * NdotL;

	// добавим отраженный свет
	float RdotVpow = max(pow(dot(reflect(-lightDir, normal), viewDir), material.shininess), 0.0);
	color += material.specular * light.specular * RdotVpow;

	// вычислим итоговый цвет пикселя на экране с учетом текстуры
	color *= texture(material.texture, Vert.texcoord) * attenuation;
}
