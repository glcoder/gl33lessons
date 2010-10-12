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
static GLuint shaderProgram = 0, colorTexture = 0;

// положение курсора и его смещение с последнего кадра
static int cursorPos[2] = {0}, cursorPosOld[2] = {0};

// матрицы преобразования
static mat4 modelViewProjectionMatrix(mat4_identity), viewMatrix(mat4_identity),
            projectionMatrix(mat4_identity), viewProjectionMatrix(mat4_identity);
static mat3 normalMatrix(mat3_identity), modelMatrix(mat3_identity), lastRot(mat3_identity);

// индекс матрицы в шейдерной программе
static GLint modelViewProjectionMatrixLocation = -1, normalMatrixLocation = -1,
             modelMatrixLocation = -1, colorTextureLocation = -1;

struct instance
{
	vec3 offset;
	vec3 direction;
};

static const uint32_t instanceCount      = 1024 * 40;
static const float    instanceSphereSize = 30.0f;
static const float    instanceSize       = 0.02f;
static float          instanceSpeed      = 1.0f;
static bool           instanceReset      = true;
static bool           instanceFollow     = false;
static Mesh           *instanceMesh      = NULL;
static GLuint         instanceVBO        = 0;

static float viewPosition = -instanceSphereSize, viewMoving = 0.0f;

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
	projectionMatrix = perspective(45.0f, aspectRatio, 0.1f, instanceSphereSize * 3.0f);

	// с помощью видовой матрицы отодвинем сцену назад
	viewMatrix = translation(0.0f, 0.0f, -instanceSphereSize);

	// получим индекс матрицы из шейдерной программы
	modelViewProjectionMatrixLocation = glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix");
	modelMatrixLocation               = glGetUniformLocation(shaderProgram, "modelMatrix");
	normalMatrixLocation              = glGetUniformLocation(shaderProgram, "normalMatrix");

	// получим индекс текстурного самплера из шейдерной программы и свяжем его с текстурным юнитом 0
	if ((colorTextureLocation = glGetUniformLocation(shaderProgram, "colorTexture")) != -1)
		glUniform1i(colorTextureLocation, 0);

	// проверка шейдерной программы на корректность
	if (!ShaderProgramValidate(shaderProgram))
		return false;

	instanceMesh = MeshCreateCube(instanceSize);

	glBindVertexArray(instanceMesh->vao);

	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(instance), NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(OFFSET_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(instance), (const GLvoid*)0);
	glVertexAttribDivisor(OFFSET_LOCATION, 1);
	glEnableVertexAttribArray(OFFSET_LOCATION);

	glVertexAttribPointer(DIRECTION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(instance), (const GLvoid*)(1 * sizeof(vec3)));
	glVertexAttribDivisor(DIRECTION_LOCATION, 1);
	glEnableVertexAttribArray(DIRECTION_LOCATION);

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

// очистка OpenGL
void GLWindowClear(const GLWindow *window)
{
	ASSERT(window);

	MeshDestroy(instanceMesh);
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

	if (modelMatrixLocation != -1)
		glUniformMatrix3fv(modelMatrixLocation, 1, GL_TRUE, instanceFollow ? mat3_identity.m : modelMatrix.m);

	if (!instanceReset)
	{
		glBindVertexArray(instanceMesh->vao);
		glDrawElementsInstanced(GL_TRIANGLES, instanceMesh->icount, GL_UNSIGNED_INT, NULL, instanceCount);
	}

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

// функция обновления
void GLWindowUpdate(const GLWindow *window, double deltaTime)
{
	ASSERT(window);
	ASSERT(deltaTime >= 0.0); // проверка на возможность бага

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

	instance *instances = (instance*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	if (!instances)
	{
		LOG_ERROR("glMapBuffer GL_ARRAY_BUFFER fail\n");
		return;
	}

	if (instanceReset)
	{
		for (uint32_t i = 0; i < instanceCount; ++i)
			instances[i].offset = sphrand() * instanceSphereSize;

		instanceSpeed = 1.0f;
	} else
	{
		for (uint32_t i = 0; i < instanceCount; ++i)
		{
			instances[i].direction = normalize(instances[(i + 1) % instanceCount].offset - instances[i].offset);
			instances[i].offset   += instances[i].direction * (float)deltaTime * instanceSpeed;
		}
	}

	instanceReset = false;

	viewMatrix = instanceFollow ? lookat(instances[0].offset, instances[1].offset, vec3_y)
		: translation(0.0f, 0.0f, viewPosition += viewMoving * (float)deltaTime);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	// рассчитаем матрицу преобразования координат вершин куба
	mat3 tempModelMatrix = instanceFollow ? mat3_identity : modelMatrix;

	modelViewProjectionMatrix = projectionMatrix * viewMatrix * mat4(tempModelMatrix);
	normalMatrix              = mat3(viewMatrix) * tempModelMatrix;

	OPENGL_CHECK_FOR_ERRORS();
}

// функция обработки ввода с клавиатуры и мыши
void GLWindowInput(const GLWindow *window)
{
	ASSERT(window);

	// центр окна
	float xCenter = 0.5f * window->width, yCenter = 0.5f * window->height;

	if (InputIsButtonDown(0))
	{
		InputGetCursorPos(cursorPos, cursorPos + 1);

		modelMatrix = arcball(vec3((float)cursorPosOld[0], (float)cursorPosOld[1], 0),
			vec3((float)cursorPos[0], (float)cursorPos[1], 0), xCenter, yCenter) * lastRot;
	} else
	{
		lastRot = modelMatrix;
		InputGetCursorPos(cursorPosOld, cursorPosOld + 1);
	}

	viewMoving = ((float)InputIsKeyDown('W') - (float)InputIsKeyDown('S'))
		* (5.0f + 10.0f * (float)InputIsKeyDown(VK_SHIFT));

	instanceSpeed += (float)InputIsKeyPressed(VK_ADD) - (float)InputIsKeyPressed(VK_SUBTRACT);

	if (InputIsKeyPressed('R'))
		instanceReset = true;

	if (InputIsKeyPressed('F'))
		instanceFollow = !instanceFollow;

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

	LoggerCreate("lesson04.log");

	if (!GLWindowCreate("lesson04", 800, 600, false))
		return 1;

	result = GLWindowMainLoop();

	GLWindowDestroy();
	LoggerDestroy();

	return result;
}
