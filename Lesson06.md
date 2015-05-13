# Урок 6 - Тени от объектов #

![http://i.imgur.com/mHg89HFl.png](http://i.imgur.com/mHg89HFl.png)

## Введение ##

В этом уроке мы рассмотрим как добавить тени от объектов на основе теневой карты (`shadow map`). Теневая карта использует буфер глубины (`depth buffer`), для того, чтобы определить, находится ли пиксель в прямой видимости источника освещения, либо он чем-то загорожен.

Одним из полезных свойств построения теневой карты является то, что ее построение не зависит от сложности геометрии на сцене, необходимо лишь получить буфер глубины для каждого из обсчитываемых источников освещения.

## Сцена и источник освещения ##

Для получения теневой карты нам необходимо получить буфер глубины сцены относительно положения и направления источника освещения. В этом уроке мы используем направленный источник освещения, чтобы получить буфер глубины сцены для такого источника нам понадобиться построить две матрицы:
  1. Матрицу вида (`view matrix`)
  1. Матрицу проекции (`projection matrix`)

Как вы можете заметить, это те же самые матрицы, которые используются в камере, поэтому удобно использовать существующие функции для работы с камерой, чтобы произвести рендер сцены относительно источника освещения.

На каждый источник освещения необходим дополнительный проход рендера сцены, с использованием камеры источника освещения. В данном уроке у нас используется только один источник освещения, поэтому понадобиться только один дополнительный проход рендера.

Стоит также отметить, что для для направленного источника освещения используется не перспективная матрица проекции, а ортогональная, ее построение выглядит следующим образом:
```
const mat4 OrthoProjection(float left, float right,
	float bottom, float top, float zNear, float zFar)
{
	const float tx = - (right + left) / (right - left),
	            ty = - (top + bottom) / (top - bottom),
	            tz = - (zFar + zNear) / (zFar - zNear);

	return mat4(2 / (right - left), 0, 0, tx,
	            0, 2 / (top - bottom), 0, ty,
	            0, 0, -2 / (zFar - zNear), tz,
	            0, 0, 0, 1);
}
```

## Буфер глубины ##

Для того, чтобы получить буфер глубины, нам необходимо:
  1. Создать текстуру для хранения буфера глубины
  1. Создать `Framebuffer Object (FBO)` и привязать к нему текстуру
  1. Настроить камеру и выполнить рендер сцены в созданный FBO

Начнем по порядку, для начала создадим текстуру для хранения буфера глубины, делается это следующим образом:
```
// функция создания текстуры для хранения буфера глубины
GLuint TextureCreateDepth(GLsizei width, GLsizei height)
{
	GLuint texture;

	// запросим у OpenGL свободный индекс текстуры
	glGenTextures(1, &texture);

	// сделаем текстуру активной
	glBindTexture(GL_TEXTURE_2D, texture);

	// установим параметры фильтрации текстуры - линейная фильтрация
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// установим параметры "оборачиваниея" текстуры - отсутствие оборачивания
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// необходимо для использования depth-текстуры как shadow map
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	// соаздем "пустую" текстуру под depth-данные
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// проверим на наличие ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return texture;
}

// создадим текстуру для хранения буфера глубины
GLuint depthTexture = TextureCreateDepth(DEPTH_TEXTURE_WIDTH, DEPTH_TEXTURE_HEIGHT);
```

Отдельно стоит сказать про размеры текстуры для хранения буфера глубины, часто, для использования этой текстуры как теневой карты, берут удвоенный размер окна, в которое происходит рендер. Но это совсем не обязательное требование, размер надо подбирать исходя из задачи.

Далее нам необходимо создать и настроить FBO, рендер сцены мы будем осуществлять с его использованием, создание FBO выглядит следующим образом:
```
// Framebuffer Object (FBO) для рендера в него буфера глубины
GLuint depthFBO = 0;
// переменная для получения состояния FBO
GLenum fboStatus;

// создаем FBO для рендера глубины в текстуру
glGenFramebuffers(1, &depthFBO);
// делаем созданный FBO текущим
glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

// отключаем вывод цвета в текущий FBO
glDrawBuffer(GL_NONE);
glReadBuffer(GL_NONE);

// указываем для текущего FBO текстуру, куда следует производить рендер глубины
glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

// проверим текущий FBO на корректность
if ((fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
{
	LOG_ERROR("glCheckFramebufferStatus error 0x%X\n", fboStatus);
	return false;
}

// возвращаем FBO по-умолчанию
glBindFramebuffer(GL_FRAMEBUFFER, 0);
```

После создания FBO и привязки к нему текстуры мы можем выполнить рендер всей сцены с использованием этого FBO:
```
// установим активный FBO
glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

// размер вьюпорта должен совпадать с размером текстуры для хранения буфера глубины
glViewport(0, 0, DEPTH_TEXTURE_WIDTH, DEPTH_TEXTURE_HEIGHT);

// отключаем вывод цвета
glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

// включаем вывод буфера глубины
glDepthMask(GL_TRUE);

// очищаем буфер глубины перед его заполнением
glClear(GL_DEPTH_BUFFER_BIT);

// отключаем отображение внешних граней объекта, оставляя внутренние
glCullFace(GL_FRONT);

// выполним рендер сцены с использованием шейдерной программы и камеры источника освещения
RenderScene(depthProgram, lightCamera);
```

Во-первых, необходимо пояснить, что при заполнении буфера глубины нет никакой необходимости обрабатывать цвет пикселя, нам необходимо узнать только его глубину по отношению к источнику освещения.

Во-вторых, рендер внутренних граней объекта помогает избежать некоторых артефактов при использовании теневой карты. Однако данный способ подходит только для замкнутых выпуклых объектов, именно такие мы и используем в этом уроке.

В-третьих, рендер сцены мы производим с использованием специальной шейдерной программы, в этой программе мы можем отключить все лишние расчеты, оставив только необходимые для получения глубины.

В минимальном варианте вершинный шефдер для такой шейдерной программы выглядит следующим образом:
```
layout(location = VERT_POSITION) in vec3 position;

// параметры преобразований
uniform struct Transform
{
	mat4 modelViewProjection;
} transform;

void main(void)
{
	// переводим координаты вершины в однородные
	gl_Position = transform.modelViewProjection * vec4(position, 1.0);
}
```

Т.к. нас интересует только глубина фрагментов, а не их цвет, то фактически фрагментный шейдер может быть пустым:
```
void main(void)
{
}
```

## Использование теневой карты ##

Как уже отмечалось ранее, смыл использования теневой карты заключается в определении является ли определенный фрагмент (пиксель) в тени другого фрагмента. Проверить мы это можем при рендере сцены, используя дополнительную матрицу источника освещения.

С помощью матрицы источника освещения мы переводим координаты вершины объекта из мировой системы координат, сначала в пространство источника освещения, а после в текстурные координаты карты глубины. Получить эту матрицу, используя камеру источника освещения, мы можем следующим образом:
```
void CameraSetupLightMatrix(GLuint program, const Camera &camera)
{
	// матрица сдвига текстурных координат
	static const mat4 bias(
		0.5f, 0.0f, 0.0f, 0.5f,
		0.0f, 0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	// расчитаем необходимые матрицы
	mat4 view           = GLFromEuler(camera.rotation) * GLTranslation(-camera.position);
	mat4 viewProjection = bias * camera.projection * view;

	// передадим матрицу источника освещения в шейдерную программу
	glUniformMatrix4fv(glGetUniformLocation(program, "transform.light"), 1, GL_TRUE, viewProjection.m);
}
```

Комментарий по поводу матрицы `bias`. Сначала мы получаем видовую матрицу источника освещения `view`, она переводит координаты вершины из мирового пространства координат в пространство координат источника освещения. Затем, используя проекционную матрицу, мы переводим координаты в однородные. Однако, как нам уже известно из предыдущих уроков, однородные координаты лежат в диапазоне `[-1, 1]`, а нам необходимо получить текстурные координаты, которые лежат в диапазоне `[0, 1]`, именно для этого используется матрица `bias`. Трансформация этой матрицы равносильна операции `a * 0.5 + 0.5` для каждого элемента трансформируемого вектора.

В результате, используя матрицу источника освещения, мы можем перевести координаты вершины из мирового пространства в текстурные координаты карты глубины в вершинном шейдере:
```
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
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
} light;

// параметры для фрагментного шейдера
out Vertex
{
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

	// передадим в фрагментный шейдер некоторые параметры
	// вычисляем текстурные координаты вершины на карте глубины
	Vert.smcoord  = transform.light * vertex;
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
```

Используя полученные координаты и теневую карту мы можем найти глубину фрагмента относительно камеры источника освещения. Обладая информацией о глубине фрагмента относительно камеры источника освещения и относительно камеры наблюдателя мы можем сравнить эти два значения и узнать, находится ли этот фрагмент в тени другого в пространстве источника освещения.

GLSL 3.30 обладает двумя возможностями, которые облегчают нам получение требуемой информации о фрагментах:
  * Самплер `sampler2DShadow`
  * Функция `textureProj`

Использование специального текстурного самплера `sampler2DShadow` позволяет изменить логику работы некоторых функций для работы с текстурой, а использование функции `textureProj` позволяет получить значение текселя текстуры с коррекцией текстурных координат.

При передаче в функцию `textureProj` в качестве текстурных координат 4х компонентного вектора для получения текселя она использует первые два компонента деленные на последний, а третий компонент используется для сравнения при тесте на затенение. Это позволит нам получить корректное значение карты глубины с учетом глубины фрагмента относительно камеры источника освещения.

При создании текстуры для хранения значений глубины мы использовали специальный параметр:
```
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
```

Использование этого параметра позволяет производить автоматический тест на затенение (`shadow test`), это происходит когда мы вызываем функцию `textureProj`. В случае если глубина фрагмента с координатами `smccord` больше чем в текстуре глубины `depthTexture` функция `textureProj` вернет `1.0`, иначе она вернет `0.0`. Итоговый фрагментый шейдер:
```
// текстурный самплер карты глубины
uniform sampler2DShadow depthTexture;

// параметры точеченого источника освещения
uniform struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
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
	vec2 texcoord;
	vec4 smcoord;
	vec3 normal;
	vec3 lightDir;
	vec3 viewDir;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

void main(void)
{
	// нормализуем полученные данные для коррекции интерполяции
	vec3 normal   = normalize(Vert.normal);
	vec3 lightDir = normalize(Vert.lightDir);
	vec3 viewDir  = normalize(Vert.viewDir);

	// коэффициент затенения
	float shadow = textureProj(depthTexture, Vert.smcoord);

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
	color *= texture(material.texture, Vert.texcoord) * shadow;
}
```

## Сглаживание краев тени ##

Для сглаживания краев тени в этом уроке используется техника `Percentage Closer Filtering (PCF)`. Данная техника заключается в том, что производится несколько тестов на затенение в окрестностях искомого фрагмента и для затенения используется среднее значение этих тестов, таким образом, граница тени будет более "мягкой".

Относительно описанного выше фрагментного шейдера эту технику можно использовать так:
```
// сглаживание границы тени, используем PCF с выборкой 3x3
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

// коэффициент затенения
float shadow  = PCF(Vert.smcoord);
```

## Исходный код и программа ##

Доступ к исходному коду уроку с проектом для MSVC можно получить двумя способами:
  * Используя SVN [lesson06](http://code.google.com/p/gl33lessons/source/browse/#svn/tags/lesson06)
  * Скачав архив (включает программу) [lesson06.zip](http://code.google.com/p/gl33lessons/downloads/detail?name=lesson06.zip)