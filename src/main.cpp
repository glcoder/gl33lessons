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
	void update(float dt);

protected:
	Texture       m_stone;
	ShaderProgram m_program;
	Mesh          m_sphere;
	RenderObject  m_object;
	Camera        m_camera;
	Material      m_material;
};

bool MainWindow::initialize()
{
	Shader m_vertex, m_fragment;

	glViewport(0, 0, m_width, m_height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (!m_sphere.load("data/cubes.bin"))
		return false;

	m_stone.create();

	if (!m_stone.load2DPNG("data/stone.png")
		|| !m_vertex.load(GL_VERTEX_SHADER, "data/normal.vs")
		|| !m_fragment.load(GL_FRAGMENT_SHADER, "data/normal.fs"))
	{
		return false;
	}

	m_program.create();
	m_program.attach(m_vertex);
	m_program.attach(m_fragment);

	if (!m_program.link())
		return false;

	m_material.setTexture(&m_stone);

	m_object.setMesh(&m_sphere);
	m_object.setMaterial(&m_material);

	vec3 position(-200.0f, 300.0f, 300.0f);
	m_camera.setPosition(position);
	m_camera.setRotation(GL::toEuler(GL::lookAt(position, vec3_zero, vec3_y)));
	m_camera.setProjection(GL::perspective(45.0f, (float)m_width / m_height, 1.0f, 1000.0f));
	//m_camera.setProjection(GL::orthographic(-10.0f, 10.0f, -10.0f, 10.0f, -100.0f, 100.0f));

	GL_CHECK_FOR_ERRORS();

	return true;
}

void MainWindow::clear()
{
	m_sphere.destroy();
	m_program.destroy();
	m_stone.destroy();
}

void MainWindow::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_program.bind();

	m_camera.setup(m_program, m_object);
	m_object.render(m_program);

	GL_CHECK_FOR_ERRORS();
}

void MainWindow::update(float dt)
{
	(void)dt;
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
