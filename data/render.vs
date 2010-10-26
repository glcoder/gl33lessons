#version 330 core

#define VERT_POSITION 0
#define VERT_TEXCOORD 1
#define VERT_NORMAL   2

layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;
layout(location = VERT_NORMAL)   in vec3 normal;

// параметры преобразований
uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;
	mat4 light;
	mat3 normal;
	vec3 viewPosition;
} transform;

// параметры точеченого источника освещения
uniform struct Light
{
	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  position;
	vec3  attenuation;
	vec3  spotDirection;
	float spotExponent;
	float spotCutoff;
} light;

// параметры для фрагментного шейдера
out Vertex {
	vec2 texcoord;
	vec4 smcoord;
	vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
} Vert;

void main(void)
{
	// переведем координаты вершины в мировую систему координат
	vec4 vertex   = transform.model * vec4(position, 1.0);

	// направление от вершины на источник освещения в мировой системе координат
	vec4 lightDir = light.position;

	// передадим в фрагментный шейдер некоторые параметры
	//
	Vert.smcoord  = transform.light * vertex;
	// передаем текстурные координаты
	Vert.texcoord = texcoord;
	// передаем нормаль в мировой системе координат
	Vert.normal   = transform.normal * normal;
	// передаем направление на источник освещения
	Vert.lightDir = vec3(lightDir);
	// передаем направление от вершины к наблюдателю в мировой системе координат
	Vert.viewDir  = transform.viewPosition - vec3(vertex);

	// переводим координаты вершины в однородные
	gl_Position = transform.viewProjection * vertex;
}
