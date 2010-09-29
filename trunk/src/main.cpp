#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "OpenGL.h"
#include "GLWindow.h"
#include "Shader.h"

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

bool Init(const GLWindow *window)
{
	ASSERT(window);

	float projectionMatrix[16];
	GLint projectionMatrixLocation, positionLocation, colorLocation;

	// устанавливаем вьюпорт на все окно
	OPENGL_CALL(glViewport(0, 0, window->width, window->height));

	// параметры OpenGL
	OPENGL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	OPENGL_CALL(glClearDepth(1.0f));

	shaderProgram  = ShaderProgramCreate();
	vertexShader   = ShaderCreate(GL_VERTEX_SHADER);
	fragmentShader = ShaderCreate(GL_FRAGMENT_SHADER);

	// загрузим и скомпилируем вершинный шейдер
	if (!ShaderLoadFromFile(vertexShader, "data/lesson.vs") || !ShaderCompile(vertexShader))
		return false;

	// загрузим и скомпилируем фрагментный шейдер
	if (!ShaderLoadFromFile(fragmentShader, "data/lesson.fs") || !ShaderCompile(fragmentShader))
		return false;

	// присоеденим загруженные шейдеры к шейдерной программе
	ShaderProgramAttach(shaderProgram, vertexShader);
	ShaderProgramAttach(shaderProgram, fragmentShader);

	// линковка шейдерной программы
	if (!ShaderProgramLink(shaderProgram))
		return false;

	ShaderProgramUse(shaderProgram);

	// создадим перспективную матрицу
	Matrix4Perspective(projectionMatrix, 45.0f,
		(float)window->width / (float)window->height, 0.5f, 5.0f);

	OPENGL_CALL(projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix"));

	if (projectionMatrixLocation != -1)
		OPENGL_CALL(glUniformMatrix4fv(projectionMatrixLocation, 1, GL_TRUE, projectionMatrix));

	// проверка на корректность шейдерной программы
	ShaderProgramValidate(shaderProgram);

	OPENGL_CALL(glGenVertexArrays(1, &meshVAO));
	OPENGL_CALL(glBindVertexArray(meshVAO));

	OPENGL_CALL(glGenBuffers(1, &meshVBO));
	OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, meshVBO));

	OPENGL_CALL(glBufferData(GL_ARRAY_BUFFER, MESH_VERTEX_COUNT * VERTEX_SIZE,
		triangleMesh, GL_STATIC_DRAW));

	OPENGL_CALL(positionLocation = glGetAttribLocation(shaderProgram, "position"));
	if (positionLocation != -1)
	{
		OPENGL_CALL(glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
			VERTEX_SIZE, (const GLvoid*)VERTEX_POSITION_OFFSET));
		OPENGL_CALL(glEnableVertexAttribArray(positionLocation));
	}

	OPENGL_CALL(colorLocation = glGetAttribLocation(shaderProgram, "color"));
	if (colorLocation != -1)
	{
		OPENGL_CALL(glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE,
			VERTEX_SIZE, (const GLvoid*)VERTEX_COLOR_OFFSET));
		OPENGL_CALL(glEnableVertexAttribArray(colorLocation));
	}

	return true;
}

void Clear(const GLWindow *window)
{
	ASSERT(window);

	OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	OPENGL_CALL(glDeleteBuffers(1, &meshVBO));

	OPENGL_CALL(glBindVertexArray(0));
	OPENGL_CALL(glDeleteVertexArrays(1, &meshVAO));

	ShaderProgramUse(0);
	ShaderProgramDestroy(shaderProgram);

	ShaderDestroy(vertexShader);
	ShaderDestroy(fragmentShader);
}

void Render(const GLWindow *window)
{
	ASSERT(window);

	// очистим буфер цвета и глубины
	OPENGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	ShaderProgramUse(shaderProgram);

	OPENGL_CALL(glBindVertexArray(meshVAO));
	OPENGL_CALL(glDrawArrays(GL_TRIANGLES, 0, MESH_VERTEX_COUNT));
}

void Update(const GLWindow *window, double dt)
{
	ASSERT(window);

	static bool fullScreenSwitched = false;

	// выход из приложения
	if (window->keyState[VK_ESCAPE])
		GLWindowDestroy();

	// переключение между оконным и полноэкранным режимом
	// осуществляется по нажатию комбинации Alt+Enter
	if (window->keyState[VK_MENU] && window->keyState[VK_RETURN])
	{
		if (!fullScreenSwitched)
		{
			GLWindowSetSize(window->width, window->height, !window->fullScreen);
			fullScreenSwitched = true;
		}
	} else
	{
		fullScreenSwitched = false;
	}
		
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int result;

	LoggerCreate("lesson02.log");

	if (!GLWindowCreate("lesson02", 800, 600, false))
		return 1;

	GLWindowSetInitFunc(Init);
	GLWindowSetClearFunc(Clear);
	GLWindowSetRenderFunc(Render);
	GLWindowSetUpdateFunc(Update);

	result = GLWindowMainLoop();

	GLWindowDestroy();
	LoggerDestroy();

	return result;
}
