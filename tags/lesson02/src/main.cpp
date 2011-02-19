#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "OpenGL.h"
#include "GLWindow.h"

// количество вершин в нашей геометрии, у нас простой треугольник
static const int MESH_VERTEX_COUNT = 3;

// размер одной вершины меша в байтах - 6 float на позицию и на цвет вершины
static const int VERTEX_SIZE = 6 * sizeof(float);

// смещения данных внутри вершины
static const int VERTEX_POSITION_OFFSET = 0;
static const int VERTEX_COLOR_OFFSET    = 3 * sizeof(float);

// пременные для хранения идентификаторов шейдерной программы и шейдеров
static GLuint shaderProgram = 0, vertexShader = 0, fragmentShader = 0;

// переменные для хранения идентификаторов VAO и VBO
static GLuint meshVAO = 0, meshVBO = 0;

// подготовим данные для вывода треугольника, всего 3 вершины
static const float triangleMesh[MESH_VERTEX_COUNT * 6] = {
	/* 1 вершина, позиция: */ -1.0f, -1.0f, -2.0f, /* цвет: */ 1.0f, 0.0f, 0.0f,
	/* 2 вершина, позиция: */  0.0f,  1.0f, -2.0f, /* цвет: */ 0.0f, 1.0f, 0.0f,
	/* 3 вершина, позиция: */  1.0f, -1.0f, -2.0f, /* цвет: */ 0.0f, 0.0f, 1.0f
};

// построение перспективной матрицы
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

// инициализаця OpenGL
bool GLWindowInit(const GLWindow *window)
{
	ASSERT(window);

	uint8_t  *shaderSource;
	uint32_t sourceLength;

	float projectionMatrix[16];
	GLint projectionMatrixLocation, positionLocation, colorLocation;

	// устанавливаем вьюпорт на все окно
	glViewport(0, 0, window->width, window->height);

	// параметры OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	// создадим шейдерную программу и шейдеры для нее
	shaderProgram  = glCreateProgram();
	vertexShader   = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// загрузим вершинный шейдер
	if (!LoadFile("data/lesson.vs", true, &shaderSource, &sourceLength))
		return false;

	// зададим шейдеру исходный код и скомпилируем его
	glShaderSource(vertexShader, 1, (const GLchar**)&shaderSource, (const GLint*)&sourceLength);
	glCompileShader(vertexShader);

	delete[] shaderSource;

	// проверим статус шейдера
	if (ShaderStatus(vertexShader, GL_COMPILE_STATUS) != GL_TRUE)
		return false;

	// загрузим фрагментный шейдер
	if (!LoadFile("data/lesson.fs", true, &shaderSource, &sourceLength))
		return false;

	// зададим шейдеру исходный код и скомпилируем его
	glShaderSource(fragmentShader, 1, (const GLchar**)&shaderSource, (const GLint*)&sourceLength);
	glCompileShader(fragmentShader);

	delete[] shaderSource;

	// проверим статус шейдера
	if (ShaderStatus(fragmentShader, GL_COMPILE_STATUS) != GL_TRUE)
		return false;

	// присоеденим загруженные шейдеры к шейдерной программе
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	// линковка шейдерной программы и проверка статуса линковки
	glLinkProgram(shaderProgram);
	if (ShaderProgramStatus(shaderProgram, GL_LINK_STATUS) != GL_TRUE)
		return false;

	// сделаем шейдер активным
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

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	// создадим и используем Vertex Array Object (VAO)
	glGenVertexArrays(1, &meshVAO);
	glBindVertexArray(meshVAO);

	// создадим и используем Vertex Buffer Object (VBO)
	glGenBuffers(1, &meshVBO);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

	// заполним VBO данными треугольника
	glBufferData(GL_ARRAY_BUFFER, MESH_VERTEX_COUNT * VERTEX_SIZE,
		triangleMesh, GL_STATIC_DRAW);

	// получим позицию атрибута 'position' из шейдера
	positionLocation = glGetAttribLocation(shaderProgram, "position");
	if (positionLocation != -1)
	{
		// назначим на атрибут параметры доступа к VBO
		glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
			VERTEX_SIZE, (const GLvoid*)VERTEX_POSITION_OFFSET);
		// разрешим использование атрибута
		glEnableVertexAttribArray(positionLocation);
	}

	// получим позицию атрибута 'color' из шейдера
	colorLocation = glGetAttribLocation(shaderProgram, "color");
	if (colorLocation != -1)
	{
		// назначим на атрибут параметры доступа к VBO
		glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE,
			VERTEX_SIZE, (const GLvoid*)VERTEX_COLOR_OFFSET);
		// разрешим использование атрибута
		glEnableVertexAttribArray(colorLocation);
	}

	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

// очистка OpenGL
void GLWindowClear(const GLWindow *window)
{
	ASSERT(window);

	// удаляем VAO и VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &meshVBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &meshVAO);

	glUseProgram(0);
	glDeleteProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

// функция рендера
void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

	// очистим буфер цвета и глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// делаем шейдерную программу активной
	glUseProgram(shaderProgram);

	// для рендеринга исопльзуем VAO
	glBindVertexArray(meshVAO);
	// рендер треугольника из VBO привязанного к VAO
	glDrawArrays(GL_TRIANGLES, 0, MESH_VERTEX_COUNT);

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

// функция обновления
void GLWindowUpdate(const GLWindow *window, double deltaTime)
{
	ASSERT(window);
	ASSERT(deltaTime >= 0.0); // проверка на возможность бага
}

// функция обработки ввода с клавиатуры и мыши
void GLWindowInput(const GLWindow *window)
{
	ASSERT(window);

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

	LoggerCreate("lesson02.log");

	if (!GLWindowCreate("lesson02", 800, 600, false))
		return 1;

	result = GLWindowMainLoop();

	GLWindowDestroy();
	LoggerDestroy();

	return result;
}
