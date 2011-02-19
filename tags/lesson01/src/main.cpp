#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "Logger.h"
#include "GLWindow.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int      result;

	LoggerCreate("lesson01.log");

	if (!GLWindowCreate("lesson01", 800, 600, false))
		return 1;

	result = GLWindowMainLoop();

	GLWindowDestroy();
	LoggerDestroy();

	return result;
}
