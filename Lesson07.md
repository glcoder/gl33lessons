# Урок 7 - Экранные эффекты #

![http://i.imgur.com/5DkVaXil.png](http://i.imgur.com/5DkVaXil.png)

## Введение ##

Темой данного урока выбраны экранные эффекты (`screen space effects`), это эффекты которые применяются к готовому изображению как к обычной картинке в редакторе изображений. Для экранных эффектов можно использовать не только информацию о цвете сцены, но также использовать карты глубины (`depth map`), карту нормалей (`normal map`) и любую информацию которую вы только захотите получить со сцены.

В общем случае реализация экранного эффекта состоит из двух шагов:
  1. Рендер сцены в текстуру (`render to texture`)
  1. Вывод полноэкранного прямоугольника с полученной текстурой (`fullscreen quad render`)

На втором шаге мы выполняем рендер полноэкранного прямоугольника с использованием специальной шейдерной программы, которая реализует экранный эффект. В эту шейдерную программу мы передаем всю информацию которую получили на первом шаге.

## Рендер в текстуру ##

Рендер в текстуру осуществляется при помощи `FBO`, для которого нам надо создать две текстуры - текстуру цвета (`color texture`) и текстуру глубины (`depth texture`):
```
// создание "пустой" текстуры с нужными параметрами
GLuint TextureCreateEmpty(GLint internalFormat, GLenum format, GLsizei width, GLsizei height)
{
	GLuint texture;

	// запросим у OpenGL свободный индекс текстуры
	glGenTextures(1, &texture);

	// сделаем текстуру активной
	glBindTexture(GL_TEXTURE_2D, texture);

	// установим параметры фильтрации текстуры - линейная фильтрация
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// установим параметры "оборачивания" текстуры - отсутствие оборачивания
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// создаем "пустую" текстуру
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);

	// проверим на наличие ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return texture;
}

// переменные для хранения индексов текстур
GLuint colorTexture = 0, depthTexture = 0;

// создадим текстуры для FBO размером с окно
colorTexture = TextureCreateEmpty(GL_RGBA8, GL_RGB, windowWidth, windowHeight);
depthTexture = TextureCreateEmpty(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
```

Теперь необходимо создать `FBO` и присоединить к нему созданные текстуры:
```
// переменная для хранения идентификатора FBO
GLuint renderFBO = 0;

// переменная для хранения состояния FBO
GLenum fboStatus;

// создаем FBO
glGenFramebuffers(1, &renderFBO);
// делаем созданный FBO текущим
glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

// присоединяем созданные текстуры к текущему FBO
glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOT_ATTACHMENT0, colorTexture, 0);
glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  depthTexture, 0);

// проверим текущий FBO на корректность
if ((fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
{
	LOG_ERROR("glCheckFramebufferStatus error 0x%X\n", fboStatus);
	return false;
}

// делаем активным дефолтный FBO
glBindFramebuffer(GL_FRAMEBUFFER, 0);
```

Для того, чтобы произвести рендер в созданный FBO мы делаем его активным и выводим все необходимые объекты:
```
// устанавливаем активный FBO
glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
// производим очистку буферов цвета и глубины
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// производим рендер сцены
RenderScene();
```

После этих действий мы получим две текстуры `colorTexture`, со значениями цвета, и `depthTexture`, со значениями глубины. Эти две текстуры мы используем для создания экранного эффекта. В данном уроке мы используем только текстуру цвета, для простых экранных эффектов, более сложные экранные эффекты потребуют также текстуру глубины и, довольно часто, текстуру нормалей.

## Рендер полноэкранного прямоугольника ##

Чтобы создать простой экранный эффект, например такой как "обесцвечивание" или "сепия", нам понадобится текстура цвета, полученная на предыдущем шаге, и возможность вывести полноэкранный прямоугольник с этой текстурой. Текстура уже есть, теперь создадим прямоугольник:
```
// макрос приведения к типу const GLvoid *
#define GL_OFFSET(x) ((const GLvoid *)(x))

// структура с описанием вершины для полноэкранного прямоугольника
struct fsqVertex
{
	float3 position;
	float2 texcoord;
};

// VAO и VBO для полноэкранного прямоугольника
GLuint fsqVAO = 0, fsqVBO = 0;

// вершины полноэкранного прямоугольника
const fsqVertex fsqVertices[6] = {
	{{-1.0f, -1.0f, 0.0f}, {0.0f,0.0f}},
	{{ 1.0f, -1.0f, 0.0f}, {1.0f,0.0f}},
	{{-1.0f,  1.0f, 0.0f}, {0.0f,1.0f}},
	{{ 1.0f, -1.0f, 0.0f}, {1.0f,0.0f}},
	{{ 1.0f,  1.0f, 0.0f}, {1.0f,1.0f}},
	{{-1.0f,  1.0f, 0.0f}, {0.0f,1.0f}}
};

// создадим VAO и VBO для рендера полноэкранного прямоугольника
glGenVertexArrays(1, &fsqVAO);
glBindVertexArray(fsqVAO);
glGenBuffers(1, &fsqVBO);

// заполняем VBO вершинными данными
glBindBuffer(GL_ARRAY_BUFFER, fsqVBO);
glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(fsqVertex), fsqVertices, GL_STATIC_DRAW);

// задаем параметры расположения вершинных атрибутов в VBO
glVertexAttribPointer(VERT_POSITION, 3, GL_FLOAT, GL_FALSE,
	sizeof(fsqVertex), GL_OFFSET(0));
glEnableVertexAttribArray(VERT_POSITION);

glVertexAttribPointer(VERT_TEXCOORD, 2, GL_FLOAT, GL_FALSE,
	sizeof(fsqVertex), GL_OFFSET(sizeof(float3)));
glEnableVertexAttribArray(VERT_TEXCOORD);
```

Весь смысл создания такого полноэкранного прямоугольника в том, что нам не придется использовать какие либо матрицы, все его вершины уже заданы в нормализованных координатах, таким образом мы можем использовать `glViewport` чтобы задать место вывода этого прямоугольника, в этом уроке мы будем использовать всю доступную область окна:
```
// устанавливаем в качестве активного дефолтный FBO
glBindFramebuffer(GL_FRAMEBUFFER, 0);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// устанавливаем шейдерную программу с реализацией экранного эффекта
ShaderProgramBind(posteffectProgram);

// устанавливаем текстуру сцены в 0-вой текстурный юнит
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, colorTexture);
glUniform1i(glGetUniformLocation(posteffectProgram, "colorTexture"), 0);

// выводим полноэкранный прямоугольник на экран
glViewport(0, 0, windowWidth, windowHeight);
glBindVertexArray(fsqVAO);
glDrawArrays(GL_TRIANGLES, 0, 6);
```

Остается только реализовать какой-нибудь экранный эффект в шейдерной программе `posteffectProgram`, загрузить ее и получить готовую картинку на экране.

## Примеры экранных эффектов ##

В исходных кодах к уроку реализованы 6 различных экранных эфектов:
  1. Обесцвечивание (`grayscale`)
  1. Сепия (`sepia`)
  1. Инверсия (`inverse`)
  1. Размытие (`blur`)
  1. Рельеф (`emboss`)
  1. Псевдо-аберрация (`aberration`)

Рассмотрим два эффекта - обесцвечивание и рельеф, последний представлен на сркиншоте к этому уроку.

Вершинный шейдер для всех экранных эффектов один и тот же, он просто получает на вход вершинные атрибуты полноэкранного прямоугольника, устанавливает позицию вершины и передает во фрагментный шейдер интерполированные текстурные координаты:
```
#version 330 core

layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;

// параметры для фрагментного шейдера
out Vertex
{
	vec2 texcoord;
} Vert;

void main(void)
{
	// передаем текстурные координаты
	Vert.texcoord = texcoord;

	// переводим координаты вершины в однородные
	gl_Position = vec4(position, 1.0);
}
```

Фрагментный шейдер для эффекта "обесцвечивания":
```
#version 330 core
// текстура цвета
uniform sampler2D colorTexture;

// параметры полученные из вершинного шейдера
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

// мера влияния каждой компоненты цвета на итоговый цвет в оттенках серого
const vec3 factor = vec3(0.27, 0.67, 0.06);

vec3 filter(in vec2 texcoord)
{
	return vec3(dot(factor, texture(colorTexture, texcoord).rgb));
}

void main(void)
{
	// обработать только левую половину изображения
	vec3 texel = Vert.texcoord.x < 0.5 ? filter(Vert.texcoord)
		: texture(colorTexture, Vert.texcoord).rgb;

	// задаем цвет фрагмента
	color = vec4(texel, 1.0);
}
```

Фрагментный шейдер для эффекта "рельеф":
```
#version 330 core
uniform sampler2D colorTexture;

// параметры полученные из вершинного шейдера
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

// размер ядра фильтра
#define KERNEL_SIZE 9

// ядро фильтра
const float kernel[KERNEL_SIZE] = float[](
	2.0,  0.0,  0.0,
	0.0, -1.0,  0.0,
	0.0,  0.0, -1.0
);

// смещение текстурных координат для фильтра
const vec2 offset[KERNEL_SIZE] = vec2[](
	vec2(-1.0,-1.0), vec2( 0.0,-1.0), vec2( 1.0,-1.0),
	vec2(-1.0, 0.0), vec2( 0.0, 0.0), vec2( 1.0, 0.0),
	vec2(-1.0, 1.0), vec2( 0.0, 1.0), vec2( 1.0, 1.0)
);

// мера влияния каждой компоненты цвета на итоговый цвет в оттенках серого
const vec3 factor = vec3(0.27, 0.67, 0.06);

// обработка фильтром указанного фрагмента
vec3 filter(in vec2 texcoord)
{
	vec2 pstep = vec2(1.0) / vec2(textureSize(colorTexture, 0));
	vec4 res   = vec4(0.5);

	for (int i = 0; i < KERNEL_SIZE; ++i)
		res += texture(colorTexture, texcoord + offset[i] * pstep) * kernel[i];

	return vec3(dot(factor, vec3(res)));
}

void main(void)
{
	// обработать только левую половину изображения
	vec3 texel = Vert.texcoord.x < 0.5 ? filter(Vert.texcoord)
		: texture(colorTexture, Vert.texcoord).rgb;

	// задаем цвет фрагмента
	color = vec4(texel, 1.0);
}
```

## Полезные ссылки ##

  1. [Image Filtering with GLSL](http://www.ozone3d.net/tutorials/image_filtering.php)
  1. [Gaussian Blur Post Processing Filter in GLSL](http://www.geeks3d.com/20100909/shader-library-gaussian-blur-post-processing-filter-in-glsl/)

## Исходный код и программа ##

Доступ к исходному коду уроку с проектом для MSVC можно получить двумя способами:
  * Используя SVN [lesson07](http://code.google.com/p/gl33lessons/source/browse/#svn/tags/lesson07)
  * Скачав архив (включает программу) [lesson07.zip](http://code.google.com/p/gl33lessons/downloads/detail?name=lesson07.zip)