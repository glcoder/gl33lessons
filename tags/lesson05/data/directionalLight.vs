layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;
layout(location = VERT_NORMAL)   in vec3 normal;

// параметры преобразований
uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;
	mat3 normal;
	vec3 viewPosition;
} transform;

// параметры источника освещения
uniform struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
} light;

// параметры для фрагментного шейдера
out Vertex
{
	vec2 texcoord;
	vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
} Vert;

void main(void)
{
	// переведем координаты вершины в мировую систему координат
	vec4 vertex   = transform.model * vec4(position, 1.0);

	// передадим в фрагментный шейдер некоторые параметры
	// передаем текстурные координаты
	Vert.texcoord = texcoord;
	// передаем нормаль в мировой системе координат
	Vert.normal   = transform.normal * normal;
	// передаем направление на источник освещения
	Vert.lightDir = vec3(light.position);
	// передаем направление от вершины к наблюдателю в мировой системе координат
	Vert.viewDir  = transform.viewPosition - vec3(vertex);

	// переводим координаты вершины в однородные
	gl_Position = transform.viewProjection * vertex;
}
