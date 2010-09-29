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

typedef bool (*GLWindowInitFunc)(const GLWindow *);
typedef void (*GLWindowClearFunc)(const GLWindow *);
typedef void (*GLWindowRenderFunc)(const GLWindow *);
typedef void (*GLWindowUpdateFunc)(const GLWindow *, double);

bool GLWindowCreate(const char *title, int width, int height, bool fullScreen = false);
void GLWindowDestroy();

void GLWindowSetSize(int width, int height, bool fullScreen = false);

void GLWindowSetInitFunc(GLWindowInitFunc init);
void GLWindowSetClearFunc(GLWindowClearFunc clear);
void GLWindowSetRenderFunc(GLWindowRenderFunc render);
void GLWindowSetUpdateFunc(GLWindowUpdateFunc update);

int GLWindowMainLoop();

#endif /* GLWINDOW_H */
