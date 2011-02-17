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
	bool loadShaderProgram(ShaderProgram &program, const char *vname, const char *fname);

	Texture       m_stone, m_depthTexture;
	ShaderProgram m_quadProgram, m_depthProgram, m_shadowmapProgram;
	Mesh          m_sphere, m_quadMesh;
	RenderObject  m_object, m_quadObject;
	Camera        m_camera, m_lightCamera;
	Material      m_material, m_quadMaterial;
	Light         m_light;
	double        m_time;
	bool          m_renderQuad;

	//Texture     m_mcolor, m_mdepth;
	Framebuffer m_defFBO, m_depthFBO;//, m_mfbo;
	//GLuint  m_mfbo;
};

bool Window::loadShaderProgram(ShaderProgram &program, const char *vname, const char *fname)
{
	Shader m_vertex, m_fragment;

	if (!m_vertex.load(GL_VERTEX_SHADER, vname) || !m_fragment.load(GL_FRAGMENT_SHADER, fname))
		return false;

	program.create();
	program.attach(m_vertex);
	program.attach(m_fragment);

	return program.link();
}

bool Window::initialize()
{
	m_time       = 0.0;
	m_renderQuad = false;

	glViewport(0, 0, m_width, m_height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	if (!m_sphere.load("data/models/cubes.bin", 0.05f))
		return false;

	m_stone.create(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

	if (!m_stone.load2DPNG("data/textures/stone.png", true)
		|| !loadShaderProgram(m_depthProgram, "data/shaders/depth.vs", "data/shaders/depth.fs")
		|| !loadShaderProgram(m_shadowmapProgram, "data/shaders/shadowmap.vs", "data/shaders/shadowmap.fs")
		|| !loadShaderProgram(m_quadProgram, "data/shaders/quad.vs", "data/shaders/quad.fs"))
	{
		return false;
	}

	m_material.setTexture(&m_stone);

	m_object.setMesh(&m_sphere);
	m_object.setMaterial(&m_material);

	m_camera.lookAt(vec3(0.0f, 1.0f, 5.0f), vec3_zero, vec3_y);
	m_camera.perspective(45.0f, (float)m_width / m_height, 0.1f, 1000.0f);

	const vec4 lightPosition(20.0f, 20.0f, 20.0f, 0.0f);
	m_light.setPosition(20.0f, 20.0f, 20.0f, 0.0f);

	m_lightCamera.lookAt(vec3(lightPosition), vec3_zero, vec3_y);
	m_lightCamera.orthographic(-10.0f, 10.0f, -10.0f, 10.0f, -100.0f, 100.0f);

	m_depthTexture.create();
	m_depthTexture.image2D(NULL, m_width * 2, m_height * 2, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);

	m_quadMesh.createFullscreenQuad();
	m_quadMaterial.setTexture(&m_depthTexture);

	m_quadObject.setMesh(&m_quadMesh);
	m_quadObject.setMaterial(&m_quadMaterial);

	m_depthFBO.create();
	m_depthFBO.attach(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture);

	m_depthFBO.setDrawBuffer(GL_NONE);
	m_depthFBO.setReadBuffer(GL_NONE);

	if (!m_depthFBO.complete())
		return false;

	/*
	GLint maxSamples;
	glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

	m_mcolor.create(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE_2D_MULTISAMPLE);
	m_mcolor.image2DMultisample(m_width, m_height, GL_RGBA8, maxSamples, GL_TRUE);

	m_mdepth.create(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE_2D_MULTISAMPLE);
	m_mdepth.image2DMultisample(m_width, m_height, GL_DEPTH_COMPONENT24, maxSamples, GL_TRUE);

	m_mfbo.create();
	m_mfbo.attach(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_mcolor);
	m_mfbo.attach(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_mdepth);

	if (!m_mfbo.complete())
		return false;
	*/

	m_defFBO.bind();

	GL_CHECK_FOR_ERRORS();

	return true;
}

void Window::clear()
{
	m_defFBO.bind();

	m_depthFBO.destroy();
	m_depthTexture.destroy();
	m_sphere.destroy();
	//m_program.destroy();
	m_stone.destroy();
}

void Window::renderScene(const ShaderProgram &program, const Camera &camera)
{
	program.bind();

	m_light.setup(program);
	m_lightCamera.setupLight(program);

	m_depthTexture.bind(1, true);
	glUniform1i(glGetUniformLocation(program.getHandle(), "depthTexture"), 1);

	camera.setup(program, m_object);
	m_object.render(program);
}

void Window::render()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	m_depthFBO.bind();

	glViewport(0, 0, m_width * 2, m_height * 2);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	renderScene(m_depthProgram, m_lightCamera);

	m_defFBO.bind();

	glViewport(0, 0, m_width, m_height);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	if (m_renderQuad)
	{
		m_quadProgram.bind();
		m_quadObject.render(m_quadProgram);
	} else
	{
		renderScene(m_shadowmapProgram, m_camera);
	}

	/*
	//glBindFramebuffer(GL_FRAMEBUFFER, m_mfbo);
	//m_mfbo.bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_program.bind();

	float time = m_time;
	glUniform1fv(glGetUniformLocation(m_program.getHandle(), "time"), 1, &time);

	m_camera.setup(m_program, m_object);
	m_object.render(m_program);

	//m_mfbo.blit(m_defFBO, 0, 0, m_width, m_height, 0, 0, m_width, m_height,
	//	GL_COLOR_BUFFER_BIT, GL_LINEAR);
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

	if (keyStates[VK_SPACE] == INPUT_KEY_PRESSED)
		m_renderQuad = !m_renderQuad;

	float moveX = (float)(keyStates['D'] > 0) - (float)(keyStates['A'] > 0);
	float moveZ = (float)(keyStates['S'] > 0) - (float)(keyStates['W'] > 0);

	m_camera.rotate(vec3(cursorRel[1], cursorRel[0], 0.0f) * 0.1f);
	m_camera.move(vec3(moveX, 0.0f, moveZ) * frameTime * 3);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Window window;
	int    result;

	Log::create("lesson.log");

	if (!window.create("lesson", 800, 600, false))
	{
		Log::destroy();
		return 1;
	}

	result = window.mainLoop();

	window.destroy();
	Log::destroy();

	return result;
}
