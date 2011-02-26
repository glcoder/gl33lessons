/* TODO:
 * 1. Fix mouse capturing after focus lost
 */

#include "common.h"
#include "OpenGL.h"
#include "Window.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "Mesh.h"
#include "RenderObject.h"
#include "Camera.h"
#include "Light.h"

#define LOAD_SHADER(p,n) \
	loadShaderProgram(p, "data/shaders/"n".vs", "data/shaders/"n".fs")

class Window : public IWindow
{
public:
	bool initialize();
	void clear();
	void render();
	void update(double frameTime);

	void renderScene(const ShaderProgram &program, const Camera &camera);

	void input(const int32_t *cursorRel, int32_t wheelRel,
		uint8_t buttons, const uint8_t *keyStates, double frameTime);

protected:
	bool loadShaderProgram(ShaderProgram &program,
		const char *vname, const char *fname);

	ShaderProgram m_renderProgram, m_depthProgram;
	Texture       m_objectTexture, m_planeTexture, m_depthTexture;
	Mesh          m_objectMesh, m_planeMesh;
	RenderObject  m_object, m_plane;
	Camera        m_mainCamera, m_lightCamera;
	Material      m_objectMaterial, m_planeMaterial;
	Light         m_light;
	Framebuffer   m_defFBO, m_depthFBO;
	double        m_time;
};

bool Window::loadShaderProgram(ShaderProgram &program,
	const char *vname, const char *fname)
{
	Shader m_vertex, m_fragment;

	if (!m_vertex.load(GL_VERTEX_SHADER, vname)
		|| !m_fragment.load(GL_FRAGMENT_SHADER, fname))
	{
		return false;
	}

	program.create();
	program.attach(m_vertex);
	program.attach(m_fragment);

	return program.link();
}

bool Window::initialize()
{
	m_time = 0.0;

	glViewport(0, 0, m_width, m_height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (!m_objectMesh.load("data/models/t80.bin")
		|| !m_planeMesh.load("data/models/plane.bin", false, vec3(3.0f, 0.0f, 3.0f)))
	{
		return false;
	}

	m_objectTexture.create(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
	m_planeTexture.create(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

	if (!m_objectTexture.load2DPNG("data/textures/t80.png", true)
		|| !m_planeTexture.load2DPNG("data/textures/gravel.png", true))
	{
		return false;
	}

	if (!LOAD_SHADER(m_renderProgram, "shadowmap")
		|| !LOAD_SHADER(m_depthProgram, "depth"))
	{
		return false;
	}

	m_objectMaterial.setTexture(&m_objectTexture);

	m_objectMaterial.setSpecular(0.1f, 0.1f, 0.1f, 1.0f);
	m_objectMaterial.setShininess(20.0f);

	m_planeMaterial.setTexture(&m_planeTexture);

	m_object.setMesh(&m_objectMesh);
	m_object.setMaterial(&m_objectMaterial);

	m_plane.setMesh(&m_planeMesh);
	m_plane.setMaterial(&m_planeMaterial);

	m_plane.setPosition(vec3(0.0f, -0.6f, 0.0f));

	const vec3 lightPosition(5.0f, 5.0f, 5.0f);
	m_light.setPosition(lightPosition.x, lightPosition.y, lightPosition.z, 0.0f);

	m_mainCamera.lookAt(lightPosition, vec3_zero, vec3_y);
	m_mainCamera.perspective(45.0f, (float)m_width / m_height, 0.1f, 50.0f);

	m_lightCamera.lookAt(lightPosition, vec3_zero, vec3_y);
	m_lightCamera.orthographic(-5.0f, 5.0f, -5.0f, 5.0f, -100.0f, 100.0f);

	m_depthTexture.create();
	m_depthTexture.image2D(NULL, m_width * 2, m_height * 2,
		GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);

	m_depthFBO.create();
	m_depthFBO.attach(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture);

	m_depthFBO.setDrawBuffer(GL_NONE);
	m_depthFBO.setReadBuffer(GL_NONE);

	if (!m_depthFBO.complete())
		return false;

	m_defFBO.bind();

	GL_CHECK_FOR_ERRORS();

	return true;
}

void Window::clear()
{
	m_defFBO.bind();
}

void Window::renderScene(const ShaderProgram &program, const Camera &camera)
{
	program.bind();

	m_light.setup(program);
	m_lightCamera.setupLight(program);

	m_depthTexture.setup(program, "depthTexture", 2, true);

	camera.setup(program, m_object);
	m_object.render(program);

	camera.setup(program, m_plane);
	m_plane.render(program);
}

void Window::render()
{
	/*
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_renderProgram.bind();
	m_light.setup(m_renderProgram);
	m_mainCamera.setup(m_renderProgram, m_object);
	m_object.render(m_renderProgram);
	*/

	m_depthFBO.bind();

	glViewport(0, 0, m_width * 2, m_height * 2);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	//glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	renderScene(m_depthProgram, m_lightCamera);

	m_defFBO.bind();

	glViewport(0, 0, m_width, m_height);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	renderScene(m_renderProgram, m_mainCamera);

	/*
	m_quadProgram.bind();

	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_FALSE);

	m_quadMaterial.setTexture(&m_colorTexture);
	m_quadObject.render(m_quadProgram);
	*/

	GL_CHECK_FOR_ERRORS();
}

void Window::update(double frameTime)
{
	m_time += frameTime;
}

void Window::input(const int32_t *cursorRel, int32_t wheelRel,
	uint8_t buttons, const uint8_t *keyStates, double frameTime)
{
	(void)wheelRel;
	(void)buttons;

	if (keyStates[VK_ESCAPE] == INPUT_KEY_PRESSED)
		m_active = false;

	float moveX = (float)(keyStates['D'] > 0) - (float)(keyStates['A'] > 0);
	float moveZ = (float)(keyStates['S'] > 0) - (float)(keyStates['W'] > 0);

	m_mainCamera.rotate(vec3(cursorRel[1], cursorRel[0], 0.0f) * 0.1f);
	m_mainCamera.move(vec3(moveX, 0.0f, moveZ) * frameTime * 3.0f);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Window window;
	int    result;

	Log::create("lesson08.log");

	if (!window.create("lesson08", 800, 600, false))
	{
		Log::destroy();
		return 1;
	}

	result = window.mainLoop();

	window.destroy();
	Log::destroy();

	return result;
}
