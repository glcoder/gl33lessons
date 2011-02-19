#ifndef GLWINDOW_H
#define GLWINDOW_H

#include "common.h"
#include "OpenGL.h"

#define INPUT_BUTTON_LEFT   0
#define INPUT_BUTTON_RIGHT  1
#define INPUT_BUTTON_MIDDLE 2

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

// стейт клавиатуры и мыши
struct Input
{
	uint8_t keyState[256];
	uint8_t buttonState[3];
	int     cursorPos[2];
};

// создание окна с поддержкой OpenGL
bool GLWindowCreate(const char *title, int width, int height, bool fullScreen = false);
// уничтожение окна
void GLWindowDestroy();

// ручное изменение размера и режима окна
void GLWindowSetSize(int width, int height, bool fullScreen = false);

bool GLWindowInit(const GLWindow *window);
void GLWindowClear(const GLWindow *window);
void GLWindowRender(const GLWindow *window);
void GLWindowUpdate(const GLWindow *window, double deltaTime);
void GLWindowInput(const GLWindow *window);

// основной цикл окна, обработка оконных сообщений и вызов необходимых функций
int GLWindowMainLoop();

// проверка нажата ли клавиша
bool InputIsKeyDown(uint8_t key);
// проверка нажата ли клавиша одинарно
bool InputIsKeyPressed(uint8_t key);

// проверка нажата ли кнопка мыши
bool InputIsButtonDown(uint8_t button);
// проверка нажата ли кнопка мыши одинарно
bool InputIsButtonClick(uint8_t button);

// получить текущее положение курсора
void InputGetCursorPos(int *x, int *y);
// задать положение курсора
void InputSetCursorPos(int x, int y);

#endif /* GLWINDOW_H */
