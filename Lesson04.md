# Урок 4 - Свет, камера, материал! Часть 1 #

![http://i.imgur.com/voKQYtQl.png](http://i.imgur.com/voKQYtQl.png)

## Введение ##

Еще одной важной темой для графических приложений и игр является освещение. Подробно останавливаться на физическом смысле и математической модели освещения я не буду, подобных статей с описаниями хватает на просторах интернета, в конце урока будут приведены ссылки на некоторые из таких статей.

В уроке будет рассмотрено создание точечного источника света с различными параметрами, который будет освещать несколько геометрических фигур на сцене с различными материалами.

Для этого урока было решено ввести небольшую математическую библиотеку для облегчения работы с матрицами и векторами. Основные понятия по матрицам были даны в предыдущем уроке, поэтому вы уже должны предоставлять какие матрицы используются, как они создаются и как передаются в шейдерную программу.

Математическая библиотека также будет дополняться по мере необходимости, так же как и исходный код очередного урока.

## Точечный источник света ##

В компьютерной графике чаще всего работают с тремя типами источников света:
  1. Точечный источник света (`point light`)
  1. Направленный источник света (`directional light`)
  1. Прожектор (`spot light`)

Как я уже отметил выше - в этом уроке будет рассмотрен только первый тип, другие два типа будут рассмотрены в следующем уроке. К точечному источнику света, с некоторой погрешностью, можно отнести обычную электрическую лампочку.

В общем случае у точечного источника света рассматривают следующие параметры:
  * Положение в пространстве (`position`)
  * Мощность фонового освещения (`ambient`)
  * Мощность рассеянного освещения (`diffuse`)
  * Мощность отраженного освещения (`specular`)
  * Коэффициенты затухания (`attenuation`)

Точечный источник испускает лучи света во все стороны, наглядно это можно представить следующим рисунком:

![http://i.imgur.com/rjynF8x.png](http://i.imgur.com/rjynF8x.png)

```
// Точечный источник освещения
struct PointLight
{
	vec4  position;
	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec3  attenuation;
};
```

Типы `vec3` и `vec4` взяты из математической библиотеки, это 3х и 4х компонентные вектора с типом `float`, соответственно. Над этими типами определены основные векторные операции, такие как сложение, умножение и т.п.

Используя функционал языка GLSL мы можем в шейдере объявить точно такую же структуру данных и потом из программы передать туда параметры точечного источника освещения.

Передавать параметры источника освещения из программы в шейдер мы будем следующим образом:
```
// функция передачи параметров источника освещения в шейдерную программу
void PointLightSetup(GLuint program, const PointLight &light)
{
	// установка параметров
	glUniform4fv(glGetUniformLocation(program, "light.position"),    1, light.position.v);
	glUniform4fv(glGetUniformLocation(program, "light.ambient"),     1, light.ambient.v);
	glUniform4fv(glGetUniformLocation(program, "light.diffuse"),     1, light.diffuse.v);
	glUniform4fv(glGetUniformLocation(program, "light.specular"),    1, light.specular.v);
	glUniform3fv(glGetUniformLocation(program, "light.attenuation"), 1, light.attenuation.v);
}
```

Обратите внимание, что доступ к элементам структуры в шейдере идет по названию конкретного экземпляра, а не по названию структуры.

## Материал объекта ##

Помимо параметров источника освещения необходимо также знать как объект будет _реагировать_ на это освещение. Объект может светится, может рассеивать свет, может отражать свет. Все это описывают параметры материала, из которого сделан объект.

Параметры материала тесно связано с параметрами света. Наиболее часто материал определяют следующими параметрами:
  * Восприятие фонового освещения (`ambient`)
  * Восприятие рассеянного освещения (`diffuse`)
  * Восприятие отраженного освещения (`specular`)
  * Самостоятельное свечение (`emission`)
  * Коэффициент блеска (`shininess`)

Дополнительно к параметрам материала будем хранить в нем текстуру объекта:
```
// Материал объекта
struct Material
{
	GLuint texture;
	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  emission;
	float shininess;
};
```

Также как и для передачи параметров точечного источника освещения в шейдере мы создадим структуру для хранения параметров материала.

По аналогии с передачей параметров точечного источника освещения из программы в шейдер мы можем передавать параметры материалы:
```
// функция передачи параметров материала в шейдерную программу
void MaterialSetup(GLuint program, const Material &material)
{
	// установка текстуры
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material.texture);
	glUniform1i(glGetUniformLocation(program, "material.texture"), 0);

	// установка параметров
	glUniform4fv(glGetUniformLocation(program, "material.ambient"),  1, material.ambient.v);
	glUniform4fv(glGetUniformLocation(program, "material.diffuse"),  1, material.diffuse.v);
	glUniform4fv(glGetUniformLocation(program, "material.specular"), 1, material.specular.v);
	glUniform4fv(glGetUniformLocation(program, "material.emission"), 1, material.emission.v);

	glUniform1fv(glGetUniformLocation(program, "material.shininess"), 1, &material.shininess);
}
```

## Камера ##

Камера в графической сцене представляет собой положение наблюдателя и то, как он видит сцену вокруг себя. Положение и направление взгляда задаются матрицей вида (`ViewMatrix`), а формат отображения задается матрицей проекции (`ProjectionMatrix`).

В этом уроке используется простая камера с возможность перемещения по сцене и обзора окружающего пространства:
```
struct Camera
{
	vec3 position;
	vec3 rotation;

	mat4 projection;
};
```

Для того, чтобы передать набор матриц трансформации вершин в шейдерную программу нам необходимо помимо камеры иметь матрицу преобразования объекта `ModelMatrix`, поэтому функция передачи параметров камеры в шейдерную программу выглядит следующим образом:
```
void CameraSetup(GLuint program, const Camera &camera, const mat4 &model)
{
	// расчитаем необходимые матрицы
	mat4 view           = GLRotation(camera.rotation) * GLTranslation(-camera.position);
	mat4 viewProjection = camera.projection * view;
	mat3 normal         = transpose(mat3(inverse(model)));

	// передаем матрицы в шейдерную программу
	glUniformMatrix4fv(glGetUniformLocation(program, "transform.model"),          1, GL_TRUE, model.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "transform.viewProjection"), 1, GL_TRUE, viewProjection.m);
	glUniformMatrix3fv(glGetUniformLocation(program, "transform.normal"),         1, GL_TRUE, normal.m);

	// передаем позицию наблюдателя (камеры) в шейдерную программу
	glUniform3fv(glGetUniformLocation(program, "transform.viewPosition"), 1, camera.position.v);
}
```

Как вы можете видеть мы ввели дополнительную матрицу `normal`, она необходима для того, чтобы переводить нормали объекта из локальной системы координат объекта в мировую, это необходимо для расчета освещения в мировой системе координат.

Стоит отметить, что при использовании `FFP` OpenGL освещение рассчитывалось не в мировой системе координат, а в видовой. На мой взгляд это не очень удобно, т.к. видовая система координат связана с камерой, а источники освещения удобно задавать в мировой системе координат и именно там и производить весь расчет.

## Расчет освещения ##

Для расчетов освещения в этом уроке используется модель освещения Фонга (`Phong shading`). Основной смысл модели в том, что итоговое освещение объекта складывается из трех компонентов:
  1. Фоновой свет (`ambient`)
  1. Рассеянный свет (`diffuse`)
  1. Отраженный свет (`specular`)

Дополнительно к этим параметрам мы добавим собственное свечение материала (`emission`), этот параметр позволяет подсветить объект даже если он не освещен ни одним источником освещения.

Соответственно каждый из компонентов рассчитывается с учетом параметров источника освещения и материала объекта. Боле подробную информацию по этой модели освещения вы можете получить [в этой заметке](http://compgraphics.info/3D/lighting/phong_reflection_model.php).

Расчет освещения может быть как повершинным (`per-vertex lighting`) так и попиксельным (`per-pixel lighting`). В данном уроке мы рассмотрим именно поиксельное освещение, оно позволяет сгладить недостаточную детализацию полигональных моделей и более точно рассчитать освещенность в каждой точке объекта. Основной расчет попиксельного освещения происходит во фрагментном шейдере.

Прежде чем приступать к расчету освещения необходимо рассчитать и передать некоторые параметры вершин из вершинного шейдера во фрагментный:
  * Нормаль к поверхности объекта в вершине (`normal`)
  * Направление падающего света, от вершины к источнику освещения (`light direction`)
  * Направление взгляда, от вершины к наблюдателю (`view direction`)
  * Расстояние от точечного источника освещения до вершины (`distance`)

Нормаль к поверхности объекта и направление падающего света используются для расчета рассеянного (`diffuse`) и отраженного (`specular`) света, однако для расчета отраженного света необходимо еще дополнительно знать направление взгляда наблюдателя. Расстояние от вершины до источника освещения необходимо для расчета общего коэффициента затухания (`attenuation`). Вершинный шейдер при этом будет таким:
```
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
	mat3 normal;
	vec3 viewPosition;
} transform;

// параметры точеченого источника освещения
uniform struct PointLight
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 attenuation;
} light;

// параметры для фрагментного шейдера
out Vertex {
	vec2  texcoord;
	vec3  normal;
	vec3  lightDir;
	vec3  viewDir;
	float distance;
} Vert;

void main(void)
{
	// переведем координаты вершины в мировую систему координат
	vec4 vertex   = transform.model * vec4(position, 1.0);

	// направление от вершины на источник освещения в мировой системе координат
	vec4 lightDir = light.position - vertex;

	// передадим во фрагментный шейдер некоторые параметры
	// передаем текстурные координаты
	Vert.texcoord = texcoord;
	// передаем нормаль в мировой системе координат
	Vert.normal   = transform.normal * normal;
	// передаем направление на источник освещения
	Vert.lightDir = vec3(lightDir);
	// передаем направление от вершины к наблюдателю в мировой системе координат
	Vert.viewDir  = transform.viewPosition - vec3(vertex);
	// передаем рассятоние от вершины до источника освещения
	Vert.distance = length(lightDir);

	// переводим координаты вершины в однородные
	gl_Position = transform.viewProjection * vertex;
}
```

После расчета и передачи необходимых параметров из вершинного шейдера во фрагментный мы можем посчитать освещенность каждой точки объекта. Для этого используем следующий фрагментный шейдер:
```
#version 330 core

#define FRAG_OUTPUT0 0

// параметры точеченого источника освещения
uniform struct PointLight
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 attenuation;
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

	// коэффициент затухания
	float attenuation = 1.0 / (light.attenuation[0] +
		light.attenuation[1] * Vert.distance +
		light.attenuation[2] * Vert.distance * Vert.distance);

	// добавим собственное свечение материала
	color = material.emission;

	// добавим фоновое освещение
	color += material.ambient * light.ambient * attenuation;

	// добавим рассеянный свет
	float NdotL = max(dot(normal, lightDir), 0.0);
	color += material.diffuse * light.diffuse * NdotL * attenuation;

	// добавим отраженный свет
	float RdotVpow = max(pow(dot(reflect(-lightDir, normal), viewDir), material.shininess), 0.0);
	color += material.specular * light.specular * RdotVpow * attenuation;

	// вычислим итоговый цвет пикселя на экране с учетом текстуры
	color *= texture(material.texture, Vert.texcoord);
}
```

В случае вычисления `NdotL` мы используем функцию `max`, чтобы отбросить отрицательные значения скалярного произведения `dot`, такое случается когда точка совсем не освещается. Если этого не учитывать расчеты рассеянной составляющей освещения становятся некорректными.

Обратите внимание, что в случае с `RdotVpow`, `max` берется не от скалярного произведения `dot` векторов, а от возведения в степень `pow`. Дело в том, что, согласно спецификации, у функции `pow(x,y)` есть неприятная особенность, результат будет неопределенным `NaN` (`not a number`), если `x < 0` либо если `x == 0` и `y <= 0`. Однако функция `max(NaN, x)` всегда возвращает `x`, таким образом мы гарантируем, что при любых входных значениях результат будет корректным.

После сложения всех составляющих освещения мы смешиваем результат со значением текселя из текстуры, получая таким образом итоговый результат в виде освещенной точки объекта с наложенной текстурой.

Результат работы программы вы уже видели на скриншоте, представленном в начале этого урока. В следующем уроке мы рассмотрим другие типы источников освещения - направленный и прожектор, а также подробнее остановимся на том, для чего нужны лэйауты (`layout`) в шейдерах и как сделать простое переключение шейдерных программ "на лету".

## Полезные ссылки ##

  1. [Модель отражения Фонга](http://compgraphics.info/3D/lighting/phong_reflection_model.php)
  1. [OpenGL Red Book Lighting](http://fly.cc.fer.hr/~unreal/theredbook/chapter06.html)
  1. [GLSL Tutorial Lighting](http://www.lighthouse3d.com/opengl/glsl/index.php?lights)
  1. [Lighting with GLSL Phong model](http://www.ozone3d.net/tutorials/glsl_lighting_phong.php)

## Исходный код ##

Доступ к исходному коду уроку с проектом для MSVC можно получить двумя способами:
  * Используя SVN [lesson04](http://code.google.com/p/gl33lessons/source/browse/#svn/tags/lesson04)
  * Скачав архив [lesson04.zip](http://code.google.com/p/gl33lessons/downloads/detail?name=lesson04.zip)