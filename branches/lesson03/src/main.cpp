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

static int    cursorPos[2] = {0,0}, cursorDelta[2] = {0,0};

static float  meshRotation[2] = {0.0f, 0.0f};

static mat4   modelViewProjectionMatrix(0.0f), viewMatrix(0.0f), projectionMatrix(0.0f);

static GLint  positionLocation = -1, texcoordLocation = -1, colorTextureLocation = -1,
              modelViewProjectionMatrixLocation = -1;

// cube mesh
static GLuint cubeVBO[3], cubeVAO;

static const uint32_t cubeVerticesCount = 24;
static const uint32_t cubeIndicesCount  = 36;

// cube vertices
static const float s = 1.0f; // size
static const float cubePositions[cubeVerticesCount][3] = {
	{-s, s, s}, { s, s, s}, { s,-s, s}, {-s,-s, s}, // front
	{ s, s,-s}, {-s, s,-s}, {-s,-s,-s}, { s,-s,-s}, // back
	{-s, s,-s}, { s, s,-s}, { s, s, s}, {-s, s, s}, // top
	{ s,-s,-s}, {-s,-s,-s}, {-s,-s, s}, { s,-s, s}, // bottom
	{-s, s,-s}, {-s, s, s}, {-s,-s, s}, {-s,-s,-s}, // left
	{ s, s, s}, { s, s,-s}, { s,-s,-s}, { s,-s, s}  // right
};

// cube texcoords
static const float cubeTexcoords[cubeVerticesCount][2] = {
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // front
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // back
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // top
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // bottom
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // left
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}  // right
};

// cube indices in CCW order
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

	glActiveTexture(GL_TEXTURE0);

	// загрузим текстуру
	colorTexture = TextureCreateFromTGA("data/texture.tga");

	if (!colorTexture)
		return false;

	// загрузим шейдерную программу
	shaderProgram = ShaderProgramCreateFromFile("data/lesson", ST_VERTEX | ST_FRAGMENT);

	if (!shaderProgram)
		return false;

	if (!ShaderProgramLink(shaderProgram))
		return false;

	ShaderProgramBind(shaderProgram);

	// создадим перспективную матрицу
	const float aspectRatio = (float)window->width / (float)window->height;
	projectionMatrix = perspective(45.0f, aspectRatio, 1.0f, 10.0f);

	// с помощью видовой матрицы отодвинем сцену назад
	viewMatrix = translation(0.0f, 0.0f, -4.0f);

	modelViewProjectionMatrixLocation = glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix");

	if ((colorTextureLocation = glGetUniformLocation(shaderProgram, "colorTexture")) != -1)
		glUniform1i(colorTextureLocation, 0);

	if (!ShaderProgramValidate(shaderProgram))
		return false;

	// создадим и используем Vertex Array Object (VAO)
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	glGenBuffers(3, cubeVBO);

	positionLocation = glGetAttribLocation(shaderProgram, "position");
	if (positionLocation != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, cubeVerticesCount * (3 * sizeof(float)),
			cubePositions, GL_STATIC_DRAW);

		glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(positionLocation);
	}

	texcoordLocation = glGetAttribLocation(shaderProgram, "texcoord");
	if (texcoordLocation != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, cubeVerticesCount * (2 * sizeof(float)),
			cubeTexcoords, GL_STATIC_DRAW);

		glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(texcoordLocation);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVBO[2]);
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(3, cubeVBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &cubeVAO);

	ShaderProgramDestroy(shaderProgram);

	TextureDestroy(colorTexture);
}

// функция рендера
void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderProgramBind(shaderProgram);

	if (modelViewProjectionMatrixLocation != -1)
		glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_TRUE, modelViewProjectionMatrix.m);

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

	meshRotation[0] += 5.0f * cursorDelta[1] * (float)deltaTime;
	meshRotation[1] += 5.0f * cursorDelta[0] * (float)deltaTime;

	cursorDelta[0] = cursorDelta[1] = 0;

	modelViewProjectionMatrix = projectionMatrix * viewMatrix
		* rotation(meshRotation[0], meshRotation[1], 0.0f);
}

// функция обработки ввода с клавиатуры и мыши
void GLWindowInput(const GLWindow *window)
{
	ASSERT(window);

	int xCenter = window->width / 2, yCenter = window->height / 2;

	InputGetCursorPos(cursorPos, cursorPos + 1);

	if (InputIsButtonDown(0))
	{
		cursorDelta[0] += xCenter - cursorPos[0];
		cursorDelta[1] += cursorPos[1] - yCenter;
	}

	InputSetCursorPos(xCenter, yCenter);

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
