#ifndef GLWINDOW_H
#define GLWINDOW_H

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "OpenGL.h"

const char GLWINDOW_CLASS_NAME[] = "GLWindow_class";
const char GLWINDOW_PROP_NAME[]  = "GLWindow_prop";

// окно с поддержкой OpenGL
class GLWindow
{
public:
	typedef void (*pRenderProc)(GLWindow *);

	GLWindow();
	~GLWindow();

	// создание окна
	bool create(const char *title, int width, int height, bool fullScreen = false);

	// установка размеров окна и полноэкранного режима
	void setSize(int width, int height, bool fullScreen = false);

	// установка процедуры рендеринга
	void setRenderProc(pRenderProc renderProc);

	// основной цикл окна
	int mainLoop();

private:
	// обработчик сообщений окна
	LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// заглушка для вызова windowProc
	static LRESULT CALLBACK sWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	DEVMODE   m_devMode;
	HINSTANCE m_hInstance;
	HWND      m_hWnd;
	HDC       m_hDC;
	HGLRC     m_hRC;

	int  m_width, m_height;
	bool m_fullScreen, m_active, m_running;

	pRenderProc m_renderProc;
};

#endif /* GLWINDOW_H */
