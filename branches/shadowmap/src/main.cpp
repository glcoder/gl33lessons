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
static GLuint shaderProgramRender = 0, shaderProgramPrepare = 0, shaderProgramOrtho = 0,
	colorTexture = 0, depthTexture = 0, depthFBO = 0;

// положение курсора и его смещение с последнего кадра
static int cursorPos[2] = {0}, rotateDelta[2] = {0}, moveDelta[2] = {0};

static const uint32_t meshCount = 3;

static Mesh     meshes[meshCount], orthoMesh;
static Material materials[meshCount], orthoMaterial;

static float3 torusRotation = {0.0f};

static Light  light;
static Camera mainCamera, orthoCamera, lightCamera;

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
	if ((shaderProgramRender = ShaderProgramCreateFromFile("data/render", ST_VERTEX | ST_FRAGMENT)) == 0)
		return false;

	// собираем созданную и загруженную шейдерную программу
	if (!ShaderProgramLink(shaderProgramRender))
		return false;

	// создадим и загрузим шейдерную программу
	if ((shaderProgramPrepare = ShaderProgramCreateFromFile("data/prepare", ST_VERTEX | ST_FRAGMENT)) == 0)
		return false;

	// собираем созданную и загруженную шейдерную программу
	if (!ShaderProgramLink(shaderProgramPrepare))
		return false;

	// создадим и загрузим шейдерную программу
	if ((shaderProgramOrtho = ShaderProgramCreateFromFile("data/ortho", ST_VERTEX | ST_FRAGMENT)) == 0)
		return false;

	// собираем созданную и загруженную шейдерную программу
	if (!ShaderProgramLink(shaderProgramOrtho))
		return false;

	// настроим точечный источник освещения
	light.position.set(5.0f, 5.0f, 5.0f, 1.0f);
	light.ambient.set(0.1f, 0.1f, 0.1f, 1.0f);
	light.diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
	light.specular.set(1.0f, 1.0f, 1.0f, 1.0f);
	light.attenuation.set(0.5f, 0.0f, 0.02f);

	// загрузим текстуры
	colorTexture = TextureCreateFromTGA("data/texture.tga");

	depthTexture = TextureCreateDepth(DT_WIDTH, DT_HEIGHT);

	// создадим примитивы и настроим материалы
	// плоскость под вращающимся тором
	MeshCreatePlane(meshes[0], vec3(0.0f, -1.6f, 0.0f), 30.0f);
	materials[0].texture = colorTexture;
	materials[0].ambient.set(0.2f, 0.2f, 0.2f, 1.0f);
	materials[0].diffuse.set(0.3f, 1.0f, 0.5f, 1.0f);
	materials[0].specular.set(0.0f, 0.0f, 0.0f, 1.0f);
	materials[0].emission.set(0.0f, 0.0f, 0.0f, 1.0f);
	materials[0].shininess = 0.0f;

	// вращающийся тор
	MeshCreateTorus(meshes[1], vec3(0.0f, 1.2f, 0.0f), 2.0f);
	materials[1].texture = colorTexture;
	materials[1].ambient.set(0.2f, 0.2f, 0.2f, 1.0f);
	materials[1].diffuse.set(0.3f, 0.5f, 1.0f, 1.0f);
	materials[1].specular.set(0.8f, 0.8f, 0.8f, 1.0f);
	materials[1].emission.set(0.0f, 0.0f, 0.0f, 1.0f);
	materials[1].shininess = 20.0f;

	// вращающийся тор
	MeshCreateTorus(meshes[2], vec3(0.0f, 1.2f, 0.0f), 1.0f);
	materials[2].texture = colorTexture;
	materials[2].ambient.set(0.2f, 0.2f, 0.2f, 1.0f);
	materials[2].diffuse.set(1.0f, 0.5f, 0.3f, 1.0f);
	materials[2].specular.set(0.8f, 0.8f, 0.8f, 1.0f);
	materials[2].emission.set(0.0f, 0.0f, 0.0f, 1.0f);
	materials[2].shininess = 20.0f;

	MeshCreateQuad(orthoMesh, vec3(0.0f, 0.0f, 0.0f), 1.0f);
	orthoMesh.rotation = mat3(GLRotationX(90.0f));

	MaterialDefault(orthoMaterial);
	orthoMaterial.texture = depthTexture;

	// создадим и настроим камеру
	const float aspectRatio = (float)window->width / (float)window->height;
	CameraLookAt(mainCamera, vec3_z * 10.0f, vec3_zero, vec3_y);
	CameraPerspective(mainCamera, 45.0f, aspectRatio, 0.5f, 50.0f);

	CameraLookAt(orthoCamera, vec3_zero, -vec3_z, vec3_y);
	CameraOrtho(orthoCamera, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	CameraLookAt(lightCamera, light.position, -light.position, vec3_y);
	CameraOrtho(lightCamera, -6.0f, 6.0f, -6.0f, 6.0f, -50.0f, 50.0f);

	glGenFramebuffers(1, &depthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		LOG_ERROR("glCheckFramebufferStatus error %d\n", fboStatus);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	ShaderProgramDestroy(shaderProgramRender);
	ShaderProgramDestroy(shaderProgramPrepare);
	ShaderProgramDestroy(shaderProgramOrtho);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &depthFBO);

	TextureDestroy(colorTexture);
	TextureDestroy(depthTexture);

	InputShowCursor(true);
}

void RenderScene(GLuint program, const Camera &camera)
{
	// делаем шейдерную программу активной
	ShaderProgramBind(program);

	LightSetup(program, light);

	CameraSetupLight(program, lightCamera);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, orthoMaterial.texture);
	glUniform1i(glGetUniformLocation(program, "depthTexture"), 1);

	for (uint32_t i = 0; i < meshCount; ++i)
	{
		CameraSetup(program, camera, MeshGetModelMatrix(meshes[i]));
		MaterialSetup(program, materials[i]);
		MeshRender(meshes[i]);
	}
}

void RenderQuad(GLuint program, const Camera &camera)
{
	// делаем шейдерную программу активной
	ShaderProgramBind(program);

	CameraSetup(program, camera, MeshGetModelMatrix(orthoMesh));
	MaterialSetup(program, orthoMaterial);
	MeshRender(orthoMesh);
}

static bool renderOrtho = false;

// функция рендера
void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glViewport(0, 0, DT_WIDTH, DT_HEIGHT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	RenderScene(shaderProgramPrepare, lightCamera);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window->width, window->height);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	if (renderOrtho)
		RenderQuad(shaderProgramOrtho, orthoCamera);
	else
		RenderScene(shaderProgramRender, mainCamera);

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

	if (InputIsKeyPressed(VK_SPACE))
		renderOrtho = !renderOrtho;

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
