#ifndef GLWINDOW_H
#define GLWINDOW_H

#include "common.h"
#include "OpenGL.h"

// структура для хранения стейта окна GLWindow
struct GLWindow
{
	// размеры окна
	int width;
	int height;

	// флаги состояние
	bool fullScreen;
	bool active;
	bool running;
};

bool GLWindowCreate(const char *title, int width, int height, bool fullScreen = false);
void GLWindowDestroy();

void GLWindowSetSize(int width, int height, bool fullScreen = false);

int GLWindowMainLoop();

#endif /* GLWINDOW_H */
