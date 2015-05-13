# Урок 5 - Свет, камера, материал! Часть 2 #

![http://i.imgur.com/Xu91yGOl.png](http://i.imgur.com/Xu91yGOl.png)

## Введение ##

В прошлом уроке мы познакомились с одним из типов источников освещения и с понятием материала объекта. В этом уроке, в дополнение к рассмотренному ранее точечному источнику освещения, будут рассмотрены следующие типы источников:
  1. направленный источник освещения (`directional light`)
  1. прожектор (`spot light`)

Помимо этого будет рассказано о том, как сделать динамическое переключение шейдерных программ, на каждый источник освещения будет отдельная шейдерная программа из вершинного и фрагментного шейдера.

## Направленный источник освещения ##

Направленный источник освещения характеризуется следующими параметрами:
  * Направление в пространстве (`direction`)
  * Мощность фонового освещения (`ambient`)
  * Мощность рассеянного освещения (`diffuse`)
  * Мощность отраженного освещения (`specular`)

Как вы можете заметить, отличие от точечного источника освещения заключается в отсутствии положения в пространстве и коэффициента затухания. Однако для того, чтобы задать направление в пространстве мы будем использовать параметр источника освещения `position`, но с нулевой четвертой компонентой (`w = 0`). По сути как положение так и направление задаются вектором, а для того, чтобы задать бесконечно удаленную от наблюдателя точку в пространстве мы приравниваем четвертую компоненту вектора к нулю. Думаю к этому моменту мы еще вернемся в одном из следующих уроков.

Направленный источник освещения можно представить следующим рисунком:

![http://i.imgur.com/VzLgddc.png](http://i.imgur.com/VzLgddc.png)

Соответственно направление источника освещения теперь не зависит от положения вершин объекта в пространстве, как это было в случае с точечным источником освещения, где направление задавалось от вершины к положению источника. Поэтому в вершинном шейдере нам необходимо будет сделать следующие изменения:
```
// передаем направление на источник освещения
Vert.lightDir = vec3(light.position);
```

Также нет необходимости рассчитывать расстояние от вершины до источника, т.к. у направленного источника освещения нет коэффициента затухания, совершенно неважно как далеко от источника находится вершина объекта, поэтому из вершинного шейдера исключается расчет параметра `Vert.distance`.

Из-за отсутствия коэффициента затухания также убирается расчет и влияние параметра `attenuation` во фрагментном шейдере. В остальном шейдер остается точно таким же как и для точечного источника освещения.

## Прожектор ##

Источник освещения типа прожектор характеризуется следующими параметрами:
  * Положение в пространстве (`position`)
  * Мощность фонового освещения (`ambient`)
  * Мощность рассеянного освещения (`diffuse`)
  * Мощность отраженного освещения (`specular`)
  * Коэффициенты затухания (`attenuation`)
  * Направление (`spot direction`)
  * Угол влияния (`spot cutoff`)
  * Коэффициент влияния (`spot exponent`)

Прожектор от части очень похож на точечный источник освещения, однако имеет еще направление (`spot direction`), угол влияния (`spot cutoff`) и коэффициент влияния (`spot exponent`).

Угол влияния (`spot cutoff`) задает угол относительно направления прожектора (`spot direction`) в котором происходит освещение, а коэффициент влияния (`spot exponent`) задает экспоненциальное затухание относительно точки в которую направлен прожектор к краям его влияния.

Таким образом, максимальная мощность освещения от прожектора будет в точке, в которую он направлен, а к краям его влияния (край влияния задается углом влияния) освещенность экспоненциально падает. Проще всего это представить следующим рисунком:

![http://i.imgur.com/DVVFwOg.png](http://i.imgur.com/DVVFwOg.png)

Как уже было отмечено выше, прожектор очень похож на точечный источник света, однако обладает влиянием только на определенную зону, это затрагивает только расчет освещенности во фрагментном шейдере, вершинный шейдер остается без изменений:
```
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
```

## Лэйауты в шейдерной программе ##

В новой версии стандарта GLSL 3.30 появился очень удобный механизм для определения индексов вершинных атрибутов прямо в исходном коде шейдера, это так называемые лэйауты (`layout`). С помощью них мы можем прописать индекс вершинного атрибута, после чего используя функцию `glGetAttribLocation` будет всегда возвращать именно этот индекс. Это очень удобно, в частности не нужно постоянно получать эти индексы, т.к. мы заранее знаем какому атрибуту какой индекс принадлежит. Используется это так:
```
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;
```

После такого определения вызов `glGetAttribLocation(program, "position")` вернет 0, а вызов `glGetAttribLocation(program, "texcoord")` вернет 1. Советую использовать этот механизм везде, где это возможно. Более подробное описание вы найдете в спецификации языка GLSL версии 3.30.

В исходных кодах к этому уроку сделана небольшая автоматизация в работе лэйаутов, в момент компиляции шейдера к нему присоединяется часть исходного кода с дефайнами (`define`), которые мы можем использовать в вершинном и фрагментном шейдерах. Отвечает за это функционал в файле `Shader.cpp`, к примеру такими деайнами являются `VERT_POSITION`, `VERT_TEXCOORD`, `VERT_NORMAL` и т.п.

Таким образом, используя лэйауты, мы можем использовать различные `VAO` не беспокоясь о том, что некоторые вершинные атрибуты будут иметь различные индексы в разных шейдерных программах.

## Переключение шейдерных программ ##

Теперь, когда мы знаем о разных типах источников освещения и знаем как отвязать описание геометрии от конкретных шейдеров при помощи лэйаутов мы можем сделать динамическое переключение шейдерных программ.

Для начала мы настроим параметры различных источников освещения:
```
// разные типы источников освещения
Light pointLight, directionalLight, spotLight;

// текущий источник освещения
Light *currentLight = NULL;

// настроим точечный источник освещения
LightDefault(pointLight, LT_POINT);
pointLight.position.set(3.0f, 3.0f, 3.0f, 1.0f);
pointLight.attenuation.set(0.5f, 0.0f, 0.02f);

// настроим направленный источник освещения
LightDefault(directionalLight, LT_DIRECTIONAL);
directionalLight.position.set(3.0f, 3.0f, 3.0f, 0.0f);

// настроим прожектор
LightDefault(spotLight, LT_SPOT);
spotLight.position.set(3.0f, 5.0f, 3.0f, 1.0f);
spotLight.spotDirection.set(-1.0f, -2.0f, -1.0f);
spotLight.spotCosCutoff = cosf(45.0f * math_radians);
spotLight.spotExponent = 5.0f;

currentLight = &pointLight;
```

После чего мы загрузим по одной шейдерной программе на каждый тип источника освещения:
```
// шейдерные программы
GLuint pointLightProgram = 0, directionalLightProgram = 0, spotLightProgram = 0;

// создадим и загрузим шейдерные программы для разных источников освещения
if ((pointLightProgram = ShaderProgramCreateFromFile("data/pointLight", ST_VERTEX | ST_FRAGMENT)) == 0
	|| !ShaderProgramLink(pointLightProgram))
{
	return false;
}

if ((directionalLightProgram = ShaderProgramCreateFromFile("data/directionalLight", ST_VERTEX | ST_FRAGMENT)) == 0
	|| !ShaderProgramLink(directionalLightProgram))
{
	return false;
}

if ((spotLightProgram = ShaderProgramCreateFromFile("data/spotLight", ST_VERTEX | ST_FRAGMENT)) == 0
	|| !ShaderProgramLink(spotLightProgram))
{
	return false;
}
```

После чего мы можем сделать выбор текущего источника освещения кнопками `F1`-`F3`, для каждого, известного нам на текущий момент, типа:
```
if (InputIsKeyPressed(VK_F1))
	currentLight = &pointLight;

if (InputIsKeyPressed(VK_F2))
	currentLight = &directionalLight;

if (InputIsKeyPressed(VK_F3))
	currentLight = &spotLight;
```

## Вывод изображения на экран ##

Заключительным этапом урока является вывод нескольких объектов на экран с использованием выбранного типа источника освещения и соответствующей ему шейдерной программы:
```
GLuint currentProgram = 0;

// выберем шейдерную программу для рендеринга
switch (currentLight->type)
{
	case LT_POINT:       currentProgram = pointLightProgram;       break;
	case LT_DIRECTIONAL: currentProgram = directionalLightProgram; break;
	case LT_SPOT:        currentProgram = spotLightProgram;        break;
	default: ASSERT(currentProgram);
}

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

ShaderProgramBind(currentProgram);

// передадим параметры источника освещения в шейдерную программу
LightSetup(currentProgram, *currentLight);

// вывод объектов сцены на экран
for (uint32_t i = 0; i < meshCount; ++i)
{
	CameraSetup(currentProgram, mainCamera, MeshGetModelMatrix(meshes[i]));
	MaterialSetup(currentProgram, materials[i]);
	MeshRender(meshes[i]);
}
```

Вот и все - используя кнопки `F1`-`F3` мы задаем активный тип источника освещения с его собственными параметрами и соответствующей ему шейдерной программой.

В следующем уроке мы рассмотрим реализацию простой тени от направленного источника освещения с использованием таких технологий как: `Shadow map` с `Percentage Closer Filtering (PCF)` и рендер в `Framebuffer Object (FBO)`. Будьте на связи :)

## Полезные ссылки ##

  1. [GLSL Phong Lighting](http://www.ozone3d.net/tutorials/glsl_lighting_phong.php)
  1. [Lighthouse 3D GLSL Lighting](http://www.lighthouse3d.com/opengl/glsl/index.php?lights)

## Исходный код и программа ##

Доступ к исходному коду уроку с проектом для MSVC можно получить двумя способами:
  * Используя SVN [lesson05](https://github.com/glcoder/gl33lessons/tree/lesson05)
  * Скачав архив [lesson05.zip](https://github.com/glcoder/gl33lessons/archive/lesson05.zip) [lesson05.tar.gz](https://github.com/glcoder/gl33lessons/archive/lesson05.tar.gz)
