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

	// состояние клавиатуры
	bool keyState[256];

	// состояние мыши
	int cursorPos[2];
	bool buttonState[3];
};

bool GLWindowCreate(const char *title, int width, int height, bool fullScreen = false);
void GLWindowDestroy();

void GLWindowSetSize(int width, int height, bool fullScreen = false);

bool GLWindowInit(const GLWindow *window);
void GLWindowClear(const GLWindow *window);
void GLWindowRender(const GLWindow *window);
void GLWindowUpdate(const GLWindow *window, double deltaTime);

int GLWindowMainLoop();

#endif /* GLWINDOW_H */
