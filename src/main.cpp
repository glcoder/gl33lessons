#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "math3d/math3d.h"
#include "OpenGL.h"
#include "GLWindow.h"
#include "Shader.h"
#include "Texture.h"

// пременные для хранения идентификаторов шейдерной программы и текстуры
static GLuint shaderProgram = 0, colorTexture = 0;

// положение курсора и его смещение с последнего кадра
static int cursorPos[2] = {0};

// матрицы преобразования
static mat4 modelViewProjectionMatrix(mat4_identity), viewMatrix(mat4_identity),
            projectionMatrix(mat4_identity), viewProjectionMatrix(mat4_identity);
static mat3 thisRot(mat3_identity), lastRot(mat3_identity);

// индексы текстуры и матрицы в шейдерной программе
static GLint colorTextureLocation = -1, modelViewProjectionMatrixLocation = -1;

// для хранения VAO и VBO связанных с кубом
static GLuint cubeVBO[3], cubeVAO;

// количество вершин куба
static const uint32_t cubeVerticesCount = 24;
// количество индексов куба
static const uint32_t cubeIndicesCount  = 36;

// координаты вершин куба
static const float s = 1.0f; // половина размера куба
static const float cubePositions[cubeVerticesCount][3] = {
	{-s, s, s}, { s, s, s}, { s,-s, s}, {-s,-s, s}, // front
	{ s, s,-s}, {-s, s,-s}, {-s,-s,-s}, { s,-s,-s}, // back
	{-s, s,-s}, { s, s,-s}, { s, s, s}, {-s, s, s}, // top
	{ s,-s,-s}, {-s,-s,-s}, {-s,-s, s}, { s,-s, s}, // bottom
	{-s, s,-s}, {-s, s, s}, {-s,-s, s}, {-s,-s,-s}, // left
	{ s, s, s}, { s, s,-s}, { s,-s,-s}, { s,-s, s}  // right
};

// текстурные координаты куба
static const float cubeTexcoords[cubeVerticesCount][2] = {
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // front
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // back
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // top
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // bottom
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // left
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}  // right
};

// индексы вершин куба в порядке поротив часовой стрелки
static const uint32_t cubeIndices[cubeIndicesCount] = {
	 0, 3, 1,  1, 3, 2, // front
	 4, 7, 5,  5, 7, 6, // back
	 8,11, 9,  9,11,10, // top
	12,15,13, 13,15,14, // bottom
	16,19,17, 17,19,18, // left
	20,23,21, 21,23,22  // right
};

// инициализаця OpenGL
bool GLWindowInit(const GLWindow *window)
{
	ASSERT(window);

	// устанавливаем вьюпорт на все окно
	glViewport(0, 0, window->width, window->height);

	// параметры OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// создадим и загрузим текстуру
	colorTexture = TextureCreateFromTGA("data/texture.tga");

	if (!colorTexture)
		return false;

	// делаем активным текстурный юнит 0 и назначаем на него текстуру
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);

	// создадим и загрузим шейдерную программу
	shaderProgram = ShaderProgramCreateFromFile("data/lesson", ST_VERTEX | ST_FRAGMENT);

	if (!shaderProgram)
		return false;

	// собираем созданную и загруженную шейдерную программу
	if (!ShaderProgramLink(shaderProgram))
		return false;

	// сделаем шейдерную программу активной
	ShaderProgramBind(shaderProgram);

	// создадим перспективную матрицу
	const float aspectRatio = (float)window->width / (float)window->height;
	projectionMatrix = perspective(45.0f, aspectRatio, 1.0f, 10.0f);

	// с помощью видовой матрицы отодвинем сцену назад
	viewMatrix = translation(0.0f, 0.0f, -4.0f);

	viewProjectionMatrix = projectionMatrix * viewMatrix;

	// получим индекс матрицы из шейдерной программы
	modelViewProjectionMatrixLocation = glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix");

	// получим индекс текстурного самплера из шейдерной программы и свяжем его с текстурным юнитом 0
	if ((colorTextureLocation = glGetUniformLocation(shaderProgram, "colorTexture")) != -1)
		glUniform1i(colorTextureLocation, 0);

	// проверка шейдерной программы на корректность
	if (!ShaderProgramValidate(shaderProgram))
		return false;

	// запросим у OpenGL свободный индекс VAO
	glGenVertexArrays(1, &cubeVAO);

	// сделаем VAO активным
	glBindVertexArray(cubeVAO);

	// создадим 3 VBO для данных куба - координаты вершин, текстурные координат и индексный буфер
	glGenBuffers(3, cubeVBO);

	// начинаем работу с буфером для координат вершин куба
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[0]);
	// поместим в буфер координаты вершин куба
	glBufferData(GL_ARRAY_BUFFER, cubeVerticesCount * (3 * sizeof(float)),
		cubePositions, GL_STATIC_DRAW);

	// укажем параметры вершинного атрибута для текущего активного VBO
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	// разрешим использование вершинного атрибута
	glEnableVertexAttribArray(POSITION_LOCATION);

	// начинаем работу с буфером для текстурных координат куба
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[1]);
	// поместим в буфер текстурные координаты куба
	glBufferData(GL_ARRAY_BUFFER, cubeVerticesCount * (2 * sizeof(float)),
		cubeTexcoords, GL_STATIC_DRAW);

	// укажем параметры вершинного атрибута для текущего активного VBO
	glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	// разрешим использование вершинного атрибута
	glEnableVertexAttribArray(TEXCOORD_LOCATION);

	// начинаем работу с индексным буфером
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVBO[2]);
	// поместим в буфер индексы вершин куба
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesCount * sizeof(uint32_t),
		cubeIndices, GL_STATIC_DRAW);

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

// очистка OpenGL
void GLWindowClear(const GLWindow *window)
{
	ASSERT(window);

	// делаем текущие VBO неактивными
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// удаляем VBO
	glDeleteBuffers(3, cubeVBO);

	// далаем текущий VAO неактивным
	glBindVertexArray(0);
	// удаляем VAO
	glDeleteVertexArrays(1, &cubeVAO);

	// удаляем шейдерную программу
	ShaderProgramDestroy(shaderProgram);

	// удаляем текстуру
	TextureDestroy(colorTexture);
}

// функция рендера
void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// делаем шейдерную программу активной
	ShaderProgramBind(shaderProgram);

	// передаем в шейдер матрицу преобразования вершин
	if (modelViewProjectionMatrixLocation != -1)
		glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_TRUE, modelViewProjectionMatrix.m);

	// выводим на экран все что относится к VAO
	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, cubeIndicesCount, GL_UNSIGNED_INT, NULL);

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

// функция обновления
void GLWindowUpdate(const GLWindow *window, double deltaTime)
{
	ASSERT(window);
	ASSERT(deltaTime >= 0.0); // проверка на возможность бага

	// рассчитаем матрицу преобразования координат вершин куба
	modelViewProjectionMatrix = viewProjectionMatrix * mat4(thisRot);
}

// функция обработки ввода с клавиатуры и мыши
void GLWindowInput(const GLWindow *window)
{
	ASSERT(window);

	// центр окна
	int xCenter = window->width / 2, yCenter = window->height / 2;

	if (InputIsButtonDown(0))
	{
		// получим положение курсора мыши
		InputGetCursorPos(cursorPos, cursorPos + 1);

		thisRot = arcball(vec3(xCenter, yCenter, 0), vec3(cursorPos[0], cursorPos[1], 0),
			xCenter, yCenter) * lastRot;
	} else
	{
		lastRot = thisRot;
		// сбросим положение курсора мыши в центр экрана
		InputSetCursorPos(xCenter, yCenter);
	}

	// выход из приложения по кнопке Esc
	if (InputIsKeyPressed(VK_ESCAPE))
		GLWindowDestroy();

	// переключение между оконным и полноэкранным режимом
	// осуществляется по нажатию комбинации Alt+Enter
	if (InputIsKeyDown(VK_MENU) && InputIsKeyPressed(VK_RETURN))
		GLWindowSetSize(window->width, window->height, !window->fullScreen);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int result;

	LoggerCreate("lesson03.log");

	if (!GLWindowCreate("lesson03", 800, 600, false))
		return 1;

	result = GLWindowMainLoop();

	GLWindowDestroy();
	LoggerDestroy();

	return result;
}
