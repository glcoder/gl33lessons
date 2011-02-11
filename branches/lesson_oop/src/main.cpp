#include "common.h"
#include "OpenGL.h"
#include "Window.h"

struct MainWindow : public Window
{
	bool initialize();
	void clear();
	void render();
	void update(float dt);
};

bool MainWindow::initialize()
{
	glViewport(0, 0, m_width, m_height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	GL_CHECK_FOR_ERRORS();

	return true;
}

void MainWindow::clear()
{
	//
}

void MainWindow::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

