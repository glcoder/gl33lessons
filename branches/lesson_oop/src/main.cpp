#include "common.h"
#include "OpenGL.h"
#include "Window.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

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
	Mesh          m_quad, m_sphere;
};

bool MainWindow::initialize()
{
	Shader m_vertex, m_fragment;

	glViewport(0, 0, m_width, m_height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	if (!m_sphere.load("data/sphere.bin"))
		return false;

	m_stone.create();

	if (!m_stone.load2DTGA("data/stone.tga")
		|| !m_vertex.load(GL_VERTEX_SHADER, "data/quad.vs")
		|| !m_fragment.load(GL_FRAGMENT_SHADER, "data/quad.fs"))
	{
		return false;
	}

	m_program.create();
	m_program.attach(m_vertex);
	m_program.attach(m_fragment);

	if (!m_program.link())
		return false;

	m_quad.createFullscreenQuad();

	GL_CHECK_FOR_ERRORS();

	return true;
}

void MainWindow::clear()
{
	m_sphere.destroy();
	m_quad.destroy();
	m_program.destroy();
	m_stone.destroy();
}

void MainWindow::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_program.bind();

	m_stone.bind(0);
	glUniform1i(glGetUniformLocation(m_program.getHandle(), "material.texture"), 0);

	m_quad.render();
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
