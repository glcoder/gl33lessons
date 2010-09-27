#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "OpenGL.h"
#include "GLWindow.h"

void render(GLWindow *)
{
	OPENGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	GLWindow window;
	int      result;

	LOG_OPEN("lesson01.log");

	if (!window.create("lesson01", 800, 600, false))
		return 1;

	window.setRenderProc(render);
	result = window.mainLoop();

	LOG_CLOSE();

	return result;
}
