#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#define DT_WIDTH  512
#define DT_HEIGHT DT_WIDTH

#include "common.h"
#include "math/math3d.h"
#include "math/mathgl.h"
#include "OpenGL.h"
#include "GLWindow.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"

// индекс шейдерной программы
static GLuint pointLightProgram = 0, directionalLightProgram = 0,
	spotLightProgram = 0, colorTexture = 0;

// положение курсора и его смещение с последнего кадра
static int cursorPos[2] = {0}, rotateDelta[2] = {0}, moveDelta[2] = {0};

static const uint32_t meshCount = 3;

static Mesh     meshes[meshCount];
static Material materials[meshCount];

static float3 torusRotation = {0.0f};

static Light  *currentLight = NULL, pointLight, directionalLight, spotLight;
static Camera mainCamera;

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

	// создадим и загрузим шейдерные программы для разных истчоников освещения
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

	// загрузим текстуры
	colorTexture = TextureCreateFromTGA("data/texture.tga");

	// создадим примитивы и настроим материалы
	// плоскость под вращающимся тором
	MeshCreatePlane(meshes[0], vec3(0.0f, -1.6f, 0.0f), 30.0f);
	MaterialDefault(materials[0]);
	materials[0].texture = colorTexture;
	materials[0].diffuse.set(0.3f, 1.0f, 0.5f, 1.0f);

	// вращающийся тор
	MeshCreateTorus(meshes[1], vec3(0.0f, 1.2f, 0.0f), 2.0f);
	MaterialDefault(materials[1]);
	materials[1].texture = colorTexture;
	materials[1].diffuse.set(0.3f, 0.5f, 1.0f, 1.0f);
	materials[1].specular.set(0.8f, 0.8f, 0.8f, 1.0f);
	materials[1].shininess = 20.0f;

	// вращающийся тор
	MeshCreateTorus(meshes[2], vec3(0.0f, 1.2f, 0.0f), 1.0f);
	MaterialDefault(materials[2]);
	materials[2].texture = colorTexture;
	materials[2].diffuse.set(1.0f, 0.5f, 0.3f, 1.0f);
	materials[2].specular.set(0.8f, 0.8f, 0.8f, 1.0f);
	materials[2].shininess = 20.0f;

	// создадим и настроим камеру
	const float aspectRatio = (float)window->width / (float)window->height;
	CameraLookAt(mainCamera, vec3(10.0f, 10.0f, 15.0f), vec3_zero, vec3_y);
	CameraPerspective(mainCamera, 45.0f, aspectRatio, 0.5f, 100.0f);

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

	ShaderProgramDestroy(pointLightProgram);
	ShaderProgramDestroy(directionalLightProgram);
	ShaderProgramDestroy(spotLightProgram);

	TextureDestroy(colorTexture);

	InputShowCursor(true);
}

// функция рендера
void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

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

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

// функция обновления
void GLWindowUpdate(const GLWindow *window, double deltaTime)
{
	ASSERT(window);
	ASSERT(deltaTime >= 0.0); // проверка на возможность бага

	// зададим углы поворота куба с учетом времени
	if ((torusRotation[0] += 30.0f * (float)deltaTime) > 360.0f)
		torusRotation[0] -= 360.0f;

	if ((torusRotation[1] += 15.0f * (float)deltaTime) > 360.0f)
		torusRotation[1] -= 360.0f;

	if ((torusRotation[2] += 7.0f * (float)deltaTime) > 360.0f)
		torusRotation[2] -= 360.0f;

	// зададим матрицу вращения куба
	meshes[1].rotation = GLFromEuler(torusRotation[0], torusRotation[1], torusRotation[2]);
	meshes[2].rotation = GLFromEuler(-torusRotation[0], torusRotation[1], -torusRotation[2]);

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

	if (InputIsKeyPressed(VK_F1))
		currentLight = &pointLight;

	if (InputIsKeyPressed(VK_F2))
		currentLight = &directionalLight;

	if (InputIsKeyPressed(VK_F3))
		currentLight = &spotLight;

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

	LoggerCreate("lesson05.log");

	if (!GLWindowCreate("lesson05", 800, 600, false))
		return 1;

	result = GLWindowMainLoop();

	GLWindowDestroy();
	LoggerDestroy();

	return result;
}
