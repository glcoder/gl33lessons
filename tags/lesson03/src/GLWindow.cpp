#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "GLWindow.h"

static const uint8_t INPUT_UP = 0, INPUT_DOWN = 1, INPUT_PRESSED = 2;

static const char GLWINDOW_CLASS_NAME[] = "GLWindow_class";

static HINSTANCE g_hInstance = NULL;
static HWND      g_hWnd      = NULL;
static HDC       g_hDC       = NULL;
static HGLRC     g_hRC       = NULL;

// обработчик сообщений окна
static LRESULT CALLBACK GLWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// хранение стейта окна и ввода
static GLWindow g_window;
static Input    g_input;

static double        g_timerFrequency = 0.0;
static LARGE_INTEGER g_qpc;

static double GetTimerTicks()
{
	QueryPerformanceCounter(&g_qpc);
	return g_timerFrequency * g_qpc.QuadPart;
}

bool GLWindowCreate(const char *title, int width, int height, bool fullScreen)
{
	ASSERT(title);
	ASSERT(width > 0);
	ASSERT(height > 0);

	WNDCLASSEX            wcx;
	PIXELFORMATDESCRIPTOR pfd;
	RECT                  rect;
	HGLRC                 hRCTemp;
	DWORD                 style, exStyle;
	int                   x, y, format;

	// обнуляем стейт окна
	memset(&g_window, 0, sizeof(g_window));

	// обнуляем стейт ввода
	memset(&g_input, 0, sizeof(g_input));

	// определим указатель на функцию создания расширенного контекста OpenGL
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

	// укажем атрибуты для создания расширенного контекста OpenGL
	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	// инциализация таймера
	QueryPerformanceFrequency(&g_qpc);
	ASSERT(g_qpc.QuadPart > 0);

	g_timerFrequency = 1.0 / g_qpc.QuadPart;

	g_hInstance = (HINSTANCE)GetModuleHandle(NULL);

	// регистрация класса окна
	memset(&wcx, 0, sizeof(wcx));
	wcx.cbSize        = sizeof(wcx);
	wcx.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcx.lpfnWndProc   = (WNDPROC)GLWindowProc;
	wcx.hInstance     = g_hInstance;
	wcx.lpszClassName = GLWINDOW_CLASS_NAME;
	wcx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);

	if (!RegisterClassEx(&wcx))
	{
		LOG_ERROR("RegisterClassEx fail (%d)\n", GetLastError());
		return false;
	}

	// стили окна
	style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	exStyle = WS_EX_APPWINDOW;

	// выровняем окно по центру экрана
	x = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
	y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	rect.left   = x;
	rect.right  = x + width;
	rect.top    = y;
	rect.bottom = y + height;

	// подгоним размер окна под стили
	AdjustWindowRectEx (&rect, style, FALSE, exStyle);

	// создаем окно
	g_hWnd = CreateWindowEx(exStyle, GLWINDOW_CLASS_NAME, title, style, rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, g_hInstance, NULL);

	if (!g_hWnd)
	{
		LOG_ERROR("CreateWindowEx fail (%d)\n", GetLastError());
		return false;
	}

	// получим дескриптор контекста окна
	g_hDC = GetDC(g_hWnd);

	if (!g_hDC)
	{
		LOG_ERROR("GetDC fail (%d)\n", GetLastError());
		return false;
	}

	// описание формата пикселей
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize      = sizeof(pfd);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;

	// запросим формат пикселей, ближайший к описанному выше
	format = ChoosePixelFormat(g_hDC, &pfd);
	if (!format || !SetPixelFormat(g_hDC, format, &pfd))
	{
		LOG_ERROR("Setting pixel format fail (%d)\n", GetLastError());
		return false;
	}

	// создадим временный контекст рендеринга
	// он нужен для получения функции wglCreateContextAttribsARB
	hRCTemp = wglCreateContext(g_hDC);
	if (!hRCTemp || !wglMakeCurrent(g_hDC, hRCTemp))
	{
		LOG_ERROR("Сreating temp render context fail (%d)\n", GetLastError());
		return false;
	}

	// получим адрес функции установки атрибутов контекста рендеринга
	OPENGL_GET_PROC(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);

	// временный контекст OpenGL нам больше не нужен
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRCTemp);

	// создадим расширенный контекст с поддержкой OpenGL 3
	g_hRC = wglCreateContextAttribsARB(g_hDC, 0, attribs);
	if (!g_hRC || !wglMakeCurrent(g_hDC, g_hRC))
	{
		LOG_ERROR("Creating render context fail (%d)\n", GetLastError());
		return false;
	}

	// вывод в лог-файл различной информации по OpenGL
	OpenGLPrintDebugInfo();

	// попробуем загрузить расширения OpenGL
	if (!OpenGLInitExtensions())
		return false;

	// зададим размеры окна
	GLWindowSetSize(width, height, fullScreen);

	return true;
}

void GLWindowDestroy()
{
	g_window.running = g_window.active = false;

	GLWindowClear(&g_window);

	// восстановим разрешение экрана
	if (g_window.fullScreen)
	{
		ChangeDisplaySettings(NULL, CDS_RESET);
		ShowCursor(TRUE);
		g_window.fullScreen = false;
	}

	// удаляем контекст рендеринга
	if (g_hRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_hRC);
		g_hRC = NULL;
	}

	// освобождаем контекст окна
	if (g_hDC)
	{
		ReleaseDC(g_hWnd, g_hDC);
		g_hDC = NULL;
	}

	// удаляем окно
	if (g_hWnd)
	{
		DestroyWindow(g_hWnd);
		g_hWnd = NULL;
	}

	// удаляем класс окна
	if (g_hInstance)
	{
		UnregisterClass(GLWINDOW_CLASS_NAME, g_hInstance);
		g_hInstance = NULL;
	}
}

void GLWindowSetSize(int width, int height, bool fullScreen)
{
	ASSERT(width > 0);
	ASSERT(height > 0);

	RECT    rect;
	DWORD   style, exStyle;
	DEVMODE devMode;
	LONG    result;
	int     x, y;

	// если мы возвращаемся из полноэкранного режима
	if (g_window.fullScreen && !fullScreen)
	{
		ChangeDisplaySettings(NULL, CDS_RESET);
		ShowCursor(TRUE);
	}

	g_window.fullScreen = fullScreen;

	// если необходим полноэкранный режим
	if (g_window.fullScreen)
	{
		memset(&devMode, 0, sizeof(devMode));
		devMode.dmSize       = sizeof(devMode);
		devMode.dmPelsWidth  = width;
		devMode.dmPelsHeight = height;
		devMode.dmBitsPerPel = GetDeviceCaps(g_hDC, BITSPIXEL);
		devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

		// попытка установить полноэкранный режим
		result = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
		if (result != DISP_CHANGE_SUCCESSFUL)
		{
			LOG_ERROR("ChangeDisplaySettings fail %dx%d (%d)\n", width, height, result);
			g_window.fullScreen = false;
		}
	}

	// если был запрошен полноэкранный режим и его удалось установить
	if (g_window.fullScreen)
	{
		ShowCursor(FALSE);

		style   = WS_POPUP;
		exStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;

		x = y = 0;
	} else // если полноэкранный режим не нужен, или его не удалось установить
	{
		style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		exStyle = WS_EX_APPWINDOW;

		// выровняем окно по центру экрана
		x = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
		y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	}

	rect.left   = x;
	rect.right  = x + width;
	rect.top    = y;
	rect.bottom = y + height;

	// подгоним размер окна под стили
	AdjustWindowRectEx (&rect, style, FALSE, exStyle);

	// установим стили окна
	SetWindowLong(g_hWnd, GWL_STYLE,   style);
	SetWindowLong(g_hWnd, GWL_EXSTYLE, exStyle);

	// обновим позицию окна
	SetWindowPos(g_hWnd, HWND_TOP, rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		SWP_FRAMECHANGED);

	// покажем окно на экране
	ShowWindow(g_hWnd, SW_SHOW);
	SetForegroundWindow(g_hWnd);
	SetFocus(g_hWnd);
	UpdateWindow(g_hWnd);

	// получим размеры окна
	GetClientRect(g_hWnd, &rect);
	g_window.width  = rect.right - rect.left;
	g_window.height = rect.bottom - rect.top;

	// центрируем курсор относительно окна
	InputSetCursorPos(g_window.width / 2, g_window.height / 2);

	OPENGL_CHECK_FOR_ERRORS();
}

int GLWindowMainLoop()
{
	MSG    msg;
	double beginFrameTime, deltaTime;

	// основной цикл окна
	g_window.running = g_window.active = GLWindowInit(&g_window);

	while (g_window.running)
	{

		// обработаем сообщения из очереди сообщений
		while (PeekMessage(&msg, g_hWnd, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				g_window.running = false;
				break;
			}
			// TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		beginFrameTime = GetTimerTicks();

		// начало обработки текущего кадра

		// чтобы не отжирать 100% CPU небольшой Sleep
		Sleep(2);

		// обработка ввода
		GLWindowInput(&g_window);

		// если окно в рабочем режиме и активно
		if (g_window.running && g_window.active)
		{
			// рендер сцены
			GLWindowRender(&g_window);
			SwapBuffers(g_hDC);

			// обновление сцены
			deltaTime = GetTimerTicks() - beginFrameTime;
			GLWindowUpdate(&g_window, deltaTime);
		}
	}

	g_window.running = g_window.active = false;
	return 0;
}

LRESULT CALLBACK GLWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		{
			g_input.cursorPos[0] = (int)LOWORD(lParam);
			g_input.cursorPos[1] = (int)HIWORD(lParam);

			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
				g_input.buttonState[0] = (msg == WM_LBUTTONDOWN ? INPUT_PRESSED : INPUT_UP);

			if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
				g_input.buttonState[1] = (msg == WM_RBUTTONDOWN ? INPUT_PRESSED : INPUT_UP);

			if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
				g_input.buttonState[2] = (msg == WM_MBUTTONDOWN ? INPUT_PRESSED : INPUT_UP);

			return FALSE;
		}

		case WM_MOUSEMOVE:
		{
			g_input.cursorPos[0] = (int)LOWORD(lParam);
			g_input.cursorPos[1] = (int)HIWORD(lParam);

			return FALSE;
		}

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			if (wParam < 256 && (lParam & 0x40000000) == 0)
				g_input.keyState[wParam] = INPUT_PRESSED;

			return FALSE;
		}

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			if (wParam < 256)
				g_input.keyState[wParam] = INPUT_UP;

			return FALSE;
		}

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		{
			g_window.active = (msg == WM_SETFOCUS);
			return FALSE;
		}

		case WM_ACTIVATE:
		{
			g_window.active = (LOWORD(wParam) != WA_INACTIVE);
			return FALSE;
		}

		case WM_CLOSE:
		{
			g_window.running = g_window.active = false;
			PostQuitMessage(0);
			return FALSE;
		}

		case WM_SYSCOMMAND:
		{
			switch (wParam & 0xFFF0)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				{
					if (g_window.fullScreen)
						return FALSE;

					break;
				}

				case SC_KEYMENU:
				case SC_TASKLIST:
				{
					return FALSE;
				}
			}

			break;
		}

		case WM_ERASEBKGND:
		{
			return FALSE;
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool InputIsKeyDown(uint8_t key)
{
	return (g_input.keyState[key] != 0);
}

bool InputIsKeyPressed(uint8_t key)
{
	bool pressed = (g_input.keyState[key] == INPUT_PRESSED);
	g_input.keyState[key] = INPUT_DOWN;
	return pressed;
}

bool InputIsButtonDown(uint8_t button)
{
	ASSERT(button < 3);

	return (g_input.buttonState[button] != 0);
}

bool InputIsButtonClick(uint8_t button)
{
	ASSERT(button < 3);

	bool pressed = (g_input.buttonState[button] == INPUT_PRESSED);
	g_input.buttonState[button] = INPUT_DOWN;
	return pressed;
}

void InputGetCursorPos(int *x, int *y)
{
	ASSERT(x);
	ASSERT(y);

	*x = g_input.cursorPos[0];
	*y = g_input.cursorPos[1];
}

void InputSetCursorPos(int x, int y)
{
	POINT pos = {x, y};
	ClientToScreen(g_hWnd, &pos);
	SetCursorPos(pos.x, pos.y);

	g_input.cursorPos[0] = x;
	g_input.cursorPos[1] = y;
}
