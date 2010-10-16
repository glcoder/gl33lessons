#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "math3d/math3d.h"
#include "OpenGL.h"
#include "GLWindow.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

// индекс шейдерной программы
static GLuint shaderProgram = 0, colorTexture = 0, normalTexture = 0;

// положение курсора и его смещение с последнего кадра
static int cursorPos[2] = {0}, cursorPosOld[2] = {0};

// матрицы преобразования
static mat4 modelMatrix(mat4_identity), viewMatrix(mat4_identity),
            projectionMatrix(mat4_identity), modelViewMatrix(mat4_identity),
            modelViewProjectionMatrix(mat4_identity), modelMatrixLast(mat4_identity);
static mat3 normalMatrix(mat3_identity);

// индекс матрицы в шейдерной программе
static GLint modelViewProjectionMatrixLocation = -1, normalMatrixLocation = -1,
             colorTextureLocation = -1, normalTextureLocation = -1,
             modelViewMatrixLocation = -1;

static Mesh *cubeMesh = NULL;

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

	normalTexture = TextureCreateFromTGA("data/normal.tga");

	if (!normalTexture)
		return false;

	// делаем активным текстурный юнит 0 и назначаем на него текстуру
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);

	// делаем активным текстурный юнит 1 и назначаем на него текстуру
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTexture);

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
	projectionMatrix = perspective(45.0f, aspectRatio, 0.1f, 20.0f);

	// с помощью видовой матрицы отодвинем сцену назад
	viewMatrix = translation(0.0f, 0.0f, -4.0f);

	// получим индекс матрицы из шейдерной программы
	modelViewProjectionMatrixLocation = glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix");
	normalMatrixLocation              = glGetUniformLocation(shaderProgram, "normalMatrix");
	modelViewMatrixLocation    = glGetUniformLocation(shaderProgram, "modelViewMatrix");

	// получим индекс текстурного самплера из шейдерной программы и свяжем его с текстурным юнитом 0
	if ((colorTextureLocation = glGetUniformLocation(shaderProgram, "colorTexture")) != -1)
		glUniform1i(colorTextureLocation, 0);

	// получим индекс текстурного самплера из шейдерной программы и свяжем его с текстурным юнитом 1
	if ((normalTextureLocation = glGetUniformLocation(shaderProgram, "normalTexture")) != -1)
		glUniform1i(normalTextureLocation, 1);

	// проверка шейдерной программы на корректность
	if (!ShaderProgramValidate(shaderProgram))
		return false;

	cubeMesh = MeshCreateCube(1.0f);

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

// очистка OpenGL
void GLWindowClear(const GLWindow *window)
{
	ASSERT(window);

	MeshDestroy(cubeMesh);
	ShaderProgramDestroy(shaderProgram);
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
		glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1,
			GL_TRUE, modelViewProjectionMatrix.m);

	if (normalMatrixLocation != -1)
		glUniformMatrix3fv(normalMatrixLocation, 1, GL_TRUE, normalMatrix.m);

	if (modelViewMatrixLocation != -1)
		glUniformMatrix4fv(modelViewMatrixLocation, 1,
			GL_TRUE, modelViewMatrix.m);

	MeshRender(cubeMesh);

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

// функция обновления
void GLWindowUpdate(const GLWindow *window, double deltaTime)
{
	ASSERT(window);
	ASSERT(deltaTime >= 0.0); // проверка на возможность бага

	modelViewMatrix           = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
	normalMatrix              = mat3(modelViewMatrix);

	OPENGL_CHECK_FOR_ERRORS();
}

// функция обработки ввода с клавиатуры и мыши
void GLWindowInput(const GLWindow *window)
{
	ASSERT(window);

	// центр окна
	float xCenter = 0.5f * window->width, yCenter = 0.5f * window->height;

	// выход из приложения по кнопке Esc
	if (InputIsKeyPressed(VK_ESCAPE))
		GLWindowDestroy();

	// переключение между оконным и полноэкранным режимом
	// осуществляется по нажатию комбинации Alt+Enter
	if (InputIsKeyDown(VK_MENU) && InputIsKeyPressed(VK_RETURN))
	{
		GLWindowSetSize(window->width, window->height, !window->fullScreen);
		//InputShowCursor(window->fullScreen ? false : true);
	}

	if (InputIsButtonDown(0))
	{
		InputGetCursorPos(cursorPos, cursorPos + 1);

		modelMatrix = arcball(vec3((float)cursorPosOld[0], (float)cursorPosOld[1], 0),
			vec3((float)cursorPos[0], (float)cursorPos[1], 0), xCenter, yCenter) * modelMatrixLast;
	} else
	{
		modelMatrixLast = modelMatrix;
		InputGetCursorPos(cursorPosOld, cursorPosOld + 1);
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int result;

	LoggerCreate("lesson04.log");

	if (!GLWindowCreate("lesson04", 800, 600, false))
		return 1;

	result = GLWindowMainLoop();

	GLWindowDestroy();
	LoggerDestroy();

	return result;
}
