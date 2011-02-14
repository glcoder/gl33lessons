#include "common.h"
#include "OpenGL.h"
#include "Window.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "RenderObject.h"
#include "Camera.h"

class MainWindow : public Window
{
public:
	bool initialize();
	void clear();
	void render();
	void update(double dt);

protected:
	bool loadShaderProgram(ShaderProgram &program, const char *vname, const char *fname);

	Texture       m_stone;
	ShaderProgram m_program;
	Mesh          m_sphere;
	RenderObject  m_object;
	Camera        m_camera;
	Material      m_material;
	double        m_time;

	Texture m_mcolor, m_mdepth;
	GLuint  m_mfbo;
};

bool MainWindow::loadShaderProgram(ShaderProgram &program, const char *vname, const char *fname)
{
	Shader m_vertex, m_fragment;

	if (!m_vertex.load(GL_VERTEX_SHADER, vname) || !m_fragment.load(GL_FRAGMENT_SHADER, fname))
		return false;

	program.create();
	program.attach(m_vertex);
	program.attach(m_fragment);

	return program.link();
}

bool MainWindow::initialize()
{
	m_time = 0.0;

	glViewport(0, 0, m_width, m_height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	if (!m_sphere.load("data/models/cube.bin"))
		return false;

	m_stone.create(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

	if (!m_stone.load2DPNG("data/textures/stone.png", true)
		|| !loadShaderProgram(m_program, "data/shaders/normal.vs", "data/shaders/normal.fs"))
	{
		return false;
	}

	m_material.setTexture(&m_stone);

	m_object.setMesh(&m_sphere);
	m_object.setMaterial(&m_material);

	vec3 position(-3.0f, 3.0f, 3.0f);
	m_camera.setPosition(position);
	m_camera.setRotation(GL::toEuler(GL::lookAt(position, vec3_zero, vec3_y)));
	m_camera.setProjection(GL::perspective(45.0f, (float)m_width / m_height, 0.1f, 20.0f));

	GLint maxSamples;
	glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

	m_mcolor.create(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE_2D_MULTISAMPLE);
	m_mcolor.image2DMultisample(m_width, m_height, GL_RGBA8, maxSamples, GL_TRUE);

	m_mdepth.create(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE_2D_MULTISAMPLE);
	m_mdepth.image2DMultisample(m_width, m_height, GL_DEPTH_COMPONENT24, maxSamples, GL_TRUE);

	glGenFramebuffers(1, &m_mfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_mfbo);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_mcolor.getHandle(), 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  m_mdepth.getHandle(), 0);

	GLenum fboStatus;
	if ((fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("glCheckFramebufferStatus error 0x%X\n", fboStatus);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GL_CHECK_FOR_ERRORS();

	return true;
}

void MainWindow::clear()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_mcolor.destroy();
	m_mdepth.destroy();
	m_sphere.destroy();
	m_program.destroy();
	m_stone.destroy();
}

void MainWindow::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_mfbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_program.bind();

	float time = m_time;
	glUniform1fv(glGetUniformLocation(m_program.getHandle(), "time"), 1, &time);

	m_camera.setup(m_program, m_object);
	m_object.render(m_program);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height,
		GL_COLOR_BUFFER_BIT, GL_LINEAR);

	GL_CHECK_FOR_ERRORS();
}

void MainWindow::update(double dt)
{
	m_time += dt * 20.0f;
	m_object.setRotation(GL::fromEuler(-m_time, m_time, 0.0f));
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	MainWindow window;
	int        result;

	Log::create("lesson.log");

	if (!window.create("lesson", 600, 600, false))
	{
		Log::destroy();
		return 1;
	}

	result = window.mainLoop();

	window.destroy();
	Log::destroy();

	return result;
}
