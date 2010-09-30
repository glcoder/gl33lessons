#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "OpenGL.h"
#include "GLWindow.h"

// количество вершин в нашей геометрии, у нас простой треугольник
const int MESH_VERTEX_COUNT = 3;

// размер одной вершины меша в байтах - 3 float на позицию вершины и 3 float на цвет вершины
const int VERTEX_SIZE = 6 * sizeof(float);

// смещения данных внутри вершины
const int VERTEX_POSITION_OFFSET = 0;
const int VERTEX_COLOR_OFFSET    = 3 * sizeof(float);

GLuint shaderProgram = 0, vertexShader = 0, fragmentShader = 0;
GLuint meshVAO = 0, meshVBO = 0;

// подготовим данные для вывода треугольника, всего 3 вершины
const float triangleMesh[MESH_VERTEX_COUNT * 6] = {
	/* 1 вершина, позиция: */ -1.0f, -1.0f, -2.0f, /* цвет: */ 1.0f, 0.0f, 0.0f,
	/* 2 вершина, позиция: */  0.0f,  1.0f, -2.0f, /* цвет: */ 0.0f, 1.0f, 0.0f,
	/* 3 вершина, позиция: */  1.0f, -1.0f, -2.0f, /* цвет: */ 0.0f, 0.0f, 1.0f
};

static void Matrix4Perspective(float *M, float fovy, float aspect, float znear, float zfar)
{
	float f = 1 / tanf(fovy / 2),
	      A = (zfar + znear) / (znear - zfar),
	      B = (2 * zfar * znear) / (znear - zfar);

	M[ 0] = f / aspect; M[ 1] =  0; M[ 2] =  0; M[ 3] =  0;
	M[ 4] = 0;          M[ 5] =  f; M[ 6] =  0; M[ 7] =  0;
	M[ 8] = 0;          M[ 9] =  0; M[10] =  A; M[11] =  B;
	M[12] = 0;          M[13] =  0; M[14] = -1; M[15] =  0;
}

bool GLWindowInit(const GLWindow *window)
{
	ASSERT(window);

	char *shaderSource;
	long sourceLength;

	float projectionMatrix[16];
	GLint projectionMatrixLocation, positionLocation, colorLocation;

	// устанавливаем вьюпорт на все окно
	glViewport(0, 0, window->width, window->height);

	// параметры OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	shaderProgram  = glCreateProgram();
	vertexShader   = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// загрузим и скомпилируем вершинный шейдер
	if (!LoadFile("data/lesson.vs", true, &shaderSource, &sourceLength))
		return false;

	glShaderSource(vertexShader, 1, &shaderSource, &sourceLength);
	glCompileShader(vertexShader);

	delete[] shaderSource;

	if (ShaderStatus(vertexShader, GL_COMPILE_STATUS) != GL_TRUE)
		return false;

	// загрузим и скомпилируем фрагментный шейдер
	if (!LoadFile("data/lesson.fs", true, &shaderSource, &sourceLength))
		return false;

	glShaderSource(fragmentShader, 1, &shaderSource, &sourceLength);
	glCompileShader(fragmentShader);

	delete[] shaderSource;

	if (ShaderStatus(fragmentShader, GL_COMPILE_STATUS) != GL_TRUE)
		return false;

	// присоеденим загруженные шейдеры к шейдерной программе
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	OPENGL_CHECK_FOR_ERRORS();

	// линковка шейдерной программы
	glLinkProgram(shaderProgram);
	if (ShaderProgramStatus(shaderProgram, GL_LINK_STATUS) != GL_TRUE)
		return false;

	glUseProgram(shaderProgram);

	// создадим перспективную матрицу
	Matrix4Perspective(projectionMatrix, 45.0f,
		(float)window->width / (float)window->height, 0.5f, 5.0f);

	projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");

	if (projectionMatrixLocation != -1)
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_TRUE, projectionMatrix);

	// проверка на корректность шейдерной программы
	glValidateProgram(shaderProgram);
	if (ShaderProgramStatus(shaderProgram, GL_VALIDATE_STATUS) != GL_TRUE)
		return false;

	OPENGL_CHECK_FOR_ERRORS();

	glGenVertexArrays(1, &meshVAO);
	glBindVertexArray(meshVAO);

	glGenBuffers(1, &meshVBO);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

	glBufferData(GL_ARRAY_BUFFER, MESH_VERTEX_COUNT * VERTEX_SIZE,
		triangleMesh, GL_STATIC_DRAW);

	positionLocation = glGetAttribLocation(shaderProgram, "position");
	if (positionLocation != -1)
	{
		glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
			VERTEX_SIZE, (const GLvoid*)VERTEX_POSITION_OFFSET);
		glEnableVertexAttribArray(positionLocation);
	}

	colorLocation = glGetAttribLocation(shaderProgram, "color");
	if (colorLocation != -1)
	{
		glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE,
			VERTEX_SIZE, (const GLvoid*)VERTEX_COLOR_OFFSET);
		glEnableVertexAttribArray(colorLocation);
	}

	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

void GLWindowClear(const GLWindow *window)
{
	ASSERT(window);

	OPENGL_CHECK_FOR_ERRORS();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &meshVBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &meshVAO);

	glUseProgram(0);
	glDeleteProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

	// очистим буфер цвета и глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);

	glBindVertexArray(meshVAO);
	glDrawArrays(GL_TRIANGLES, 0, MESH_VERTEX_COUNT);

	OPENGL_CHECK_FOR_ERRORS();
}

void GLWindowUpdate(const GLWindow *window, double deltaTime)
{
	ASSERT(window);

	static bool screeModeSwitched = false;

	// выход из приложения по кнопке Esc
	if (window->keyState[VK_ESCAPE])
		GLWindowDestroy();

	// переключение между оконным и полноэкранным режимом
	// осуществляется по нажатию комбинации Alt+Enter
	if (window->keyState[VK_MENU] && window->keyState[VK_RETURN])
	{
		if (!screeModeSwitched)
		{
			GLWindowSetSize(window->width, window->height, !window->fullScreen);
			screeModeSwitched = true;
		}
	} else
	{
		screeModeSwitched = false;
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int result;

	LoggerCreate("lesson02.log");

	if (!GLWindowCreate("lesson02", 800, 600, false))
		return 1;

	result = GLWindowMainLoop();

	GLWindowDestroy();
	LoggerDestroy();

	return result;
}
