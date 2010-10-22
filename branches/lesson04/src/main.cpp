#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "math/math3d.h"
#include "math/mathgl.h"
#include "OpenGL.h"
#include "GLWindow.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"

// параметры точеченого источника освещения
struct PointLight
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 attenuation;
};

// параметры материала
struct Material
{
	GLuint texture;

	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  emission;
	float shininess;
};

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

// индекс шейдерной программы
static GLuint shaderProgram = 0, colorTexture = 0, blankTexture = 0;

// положение курсора и его смещение с последнего кадра
static int cursorPos[2] = {0}, rotateDelta[2] = {0}, moveDelta[2] = {0};

static const uint32_t meshCount = 3;

static Mesh     meshes[meshCount];
static Material materials[meshCount];

static float3 cubeRotation = {0.0f};

static PointLight pointLight;
static Camera     mainCamera;

// инициализаця OpenGL
bool GLWindowInit(const GLWindow *window)
{
	ASSERT(window);

	// спрячем курсор
	InputShowCursor(false);

	// устанавливаем вьюпорт на все окно
	glViewport(0, 0, window->width, window->height);

	// параметры OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// создадим и загрузим шейдерную программу
	if ((shaderProgram = ShaderProgramCreateFromFile("data/lesson", ST_VERTEX | ST_FRAGMENT)) == 0)
		return false;

	// собираем созданную и загруженную шейдерную программу
	if (!ShaderProgramLink(shaderProgram))
		return false;

	// сделаем шейдерную программу активной
	ShaderProgramBind(shaderProgram);

	// проверка шейдерной программы на корректность
	if (!ShaderProgramValidate(shaderProgram))
		return false;

	// настроим точечный источник освещения
	pointLight.position.set(3.0f, 3.0f, 3.0f, 1.0f);
	pointLight.ambient.set(0.1f, 0.1f, 0.1f, 1.0f);
	pointLight.diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
	pointLight.specular.set(1.0f, 1.0f, 1.0f, 1.0f);
	pointLight.attenuation.set(0.5f, 0.0f, 0.02f);

	// загрузим текстуры
	colorTexture = TextureCreateFromTGA("data/texture.tga");
	blankTexture = TextureCreateFromTGA("data/blank.tga");

	// создадим примитивы и настроим материалы
	// вращающийся тор
	MeshCreateTorus(meshes[0], vec3(0.0f, 1.2f, 0.0f), 2.0f);
	materials[0].texture = colorTexture;
	materials[0].ambient.set(0.2f, 0.2f, 0.2f, 1.0f);
	materials[0].diffuse.set(0.3f, 0.5f, 1.0f, 1.0f);
	materials[0].specular.set(0.8f, 0.8f, 0.8f, 1.0f);
	materials[0].emission.set(0.0f, 0.0f, 0.0f, 1.0f);
	materials[0].shininess = 20.0f;

	// плоскость под вращающимся тором
	MeshCreatePlane(meshes[1], vec3(0.0f, -1.6f, 0.0f), 30.0f);
	materials[1].texture = colorTexture;
	materials[1].ambient.set(0.2f, 0.2f, 0.2f, 1.0f);
	materials[1].diffuse.set(0.3f, 1.0f, 0.5f, 1.0f);
	materials[1].specular.set(0.0f, 0.0f, 0.0f, 1.0f);
	materials[1].emission.set(0.0f, 0.0f, 0.0f, 1.0f);
	materials[1].shininess = 0.0f;

	// сфера на месте источника освещения
	MeshCreateSphere(meshes[2], pointLight.position, 0.2f);
	materials[2].texture = blankTexture;
	materials[2].ambient.set(0.2f, 0.2f, 0.2f, 1.0f);
	materials[2].diffuse.set(0.8f, 0.8f, 0.8f, 1.0f);
	materials[2].specular.set(0.0f, 0.0f, 0.0f, 1.0f);
	materials[2].emission.set(1.0f, 1.0f, 1.0f, 1.0f);
	materials[2].shininess = 0.0f;

	// создадим и настроим камеру
	const float aspectRatio = (float)window->width / (float)window->height;
	CameraCreate(mainCamera, 0.0f, 0.0f, 12.0f);
	CameraPerspective(mainCamera, 45.0f, aspectRatio, 0.1f, 50.0f);

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

// очистка OpenGL
void GLWindowClear(const GLWindow *window)
{
	ASSERT(window);

	for (uint32_t i = 0; i < meshCount; ++i)
		MeshDestroy(meshes[i]);

	ShaderProgramDestroy(shaderProgram);

	TextureDestroy(colorTexture);
	TextureDestroy(blankTexture);

	InputShowCursor(true);
}

// функция рендера
void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// делаем шейдерную программу активной
	ShaderProgramBind(shaderProgram);

	PointLightSetup(shaderProgram, pointLight);

	for (uint32_t i = 0; i < meshCount; ++i)
	{
		CameraSetup(shaderProgram, mainCamera, MeshGetModelMatrix(meshes[i]));
		MaterialSetup(shaderProgram, materials[i]);
		MeshRender(meshes[i]);
	}

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

// функция обновления
void GLWindowUpdate(const GLWindow *window, double deltaTime)
{
	ASSERT(window);
	ASSERT(deltaTime >= 0.0); // проверка на возможность бага

	// зададим углы поворота куба с учетом времени
	if ((cubeRotation[0] += 30.0f * (float)deltaTime) > 360.0f)
		cubeRotation[0] -= 360.0f;

	if ((cubeRotation[1] += 15.0f * (float)deltaTime) > 360.0f)
		cubeRotation[1] -= 360.0f;

	if ((cubeRotation[2] += 7.0f * (float)deltaTime) > 360.0f)
		cubeRotation[2] -= 360.0f;

	// зададим матрицу вращения куба
	meshes[0].rotation = GLRotation(cubeRotation[0], cubeRotation[1], cubeRotation[2]);

	// вращаем камеру
	CameraRotate(mainCamera, (float)deltaTime * rotateDelta[1], (float)deltaTime * rotateDelta[0], 0.0f);
	// двигаем камеру
	CameraMove(mainCamera, (float)deltaTime * moveDelta[0], 0.0f, (float)deltaTime * moveDelta[1]);

	rotateDelta[0] = rotateDelta[1] = 0;
	moveDelta[0] = moveDelta[1] = 0;

	OPENGL_CHECK_FOR_ERRORS();
}

// функция обработки ввода с клавиатуры и мыши
void GLWindowInput(const GLWindow *window)
{
	ASSERT(window);

	// центр окна
	int32_t xCenter = window->width / 2, yCenter = window->height / 2;

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

	moveDelta[0] = 10 * ((int)InputIsKeyDown('D') - (int)InputIsKeyDown('A'));
	moveDelta[1] = 10 * ((int)InputIsKeyDown('S') - (int)InputIsKeyDown('W'));

	InputGetCursorPos(cursorPos, cursorPos + 1);
	rotateDelta[0] += cursorPos[0] - xCenter;
	rotateDelta[1] += cursorPos[1] - yCenter;
	InputSetCursorPos(xCenter, yCenter);
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
