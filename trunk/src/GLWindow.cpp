#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "GLWindow.h"

const char GLWINDOW_CLASS_NAME[] = "GLWindow_class";

GLWindow  g_window;

HINSTANCE g_hInstance;
HWND      g_hWnd;
HDC       g_hDC;
HGLRC     g_hRC;

// обработчик сообщений окна
LRESULT CALLBACK GLWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

	g_hInstance = static_cast<HINSTANCE>(GetModuleHandle(NULL));

	// регистрация класса окна
	memset(&wcx, 0, sizeof(wcx));
	wcx.cbSize        = sizeof(wcx);
	wcx.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcx.lpfnWndProc   = reinterpret_cast<WNDPROC>(GLWindowProc);
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
	wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(
		wglGetProcAddress("wglCreateContextAttribsARB"));

	if (!wglCreateContextAttribsARB)
	{
		LOG_ERROR("wglCreateContextAttribsARB fail (%d)\n", GetLastError());
		wglDeleteContext(hRCTemp);
		return false;
	}

	// создадим расширенный контекст с поддержкой OpenGL 3
	g_hRC = wglCreateContextAttribsARB(g_hDC, 0, attribs);
	if (!g_hRC || !wglMakeCurrent(g_hDC, g_hRC))
	{
		LOG_ERROR("Creating render context fail (%d)\n", GetLastError());
		wglDeleteContext(hRCTemp);
		return false;
	}

	// больше нам временный контекст OpenGL не нужен
	wglDeleteContext(hRCTemp);

	// выведем в лог немного информации о контексте OpenGL
	int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	LOG_DEBUG("OpenGL render context information:\n"
		"  Renderer       : %s\n"
		"  Vendor         : %s\n"
		"  Version        : %s\n"
		"  GLSL version   : %s\n"
		"  OpenGL version : %d.%d\n",
		(const char*)glGetString(GL_RENDERER),
		(const char*)glGetString(GL_VENDOR),
		(const char*)glGetString(GL_VERSION),
		(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION),
		major, minor
	);

	// зададим размеры окна
	GLWindowSetSize(width, height, fullScreen);

	return true;
}

void GLWindowDestroy()
{
	// восстановим разрешение экрана
	if (g_window.fullScreen)
	{
		ChangeDisplaySettings(NULL, CDS_RESET);
		ShowCursor(TRUE);
	}

	// удаляем контекст рендеринга
	if (g_hRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_hRC);
	}

	// освобождаем контекст окна
	if (g_hDC)
		ReleaseDC(g_hWnd, g_hDC);

	// удаляем окно
	if (g_hWnd)
		DestroyWindow(g_hWnd);

	// удаляем класс окна
	if (g_hInstance)
		UnregisterClass(GLWINDOW_CLASS_NAME, g_hInstance);
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

	// устанавливаем вьюпорт на все окно
	OPENGL_CALL(glViewport(0, 0, g_window.width, g_window.height));

	// центрируем курсор относительно окна
	SetCursorPos(x + g_window.width / 2, y + g_window.height / 2);

	OPENGL_CHECK_FOR_ERRORS();
}

int GLWindowMainLoop()
{
	MSG msg;

	// основной цикл окна
	g_window.running = g_window.active = true;

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

		// если окно в рабочем режиме и активно
		if (g_window.running && g_window.active)
		{
			OPENGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			SwapBuffers(g_hDC);
		}

		Sleep(2);
	}

	g_window.running = g_window.active = false;
	return 0;
}

LRESULT CALLBACK GLWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				g_window.running = false;

			if (wParam == VK_F1)
				GLWindowSetSize(g_window.width, g_window.height, !g_window.fullScreen);

			return FALSE;

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			g_window.active = (msg == WM_SETFOCUS);
			return FALSE;

		case WM_ACTIVATE:
			g_window.active = (LOWORD(wParam) == WA_INACTIVE);
			return FALSE;

		case WM_CLOSE:
			g_window.running = g_window.active = false;
			PostQuitMessage(0);
			return FALSE;

		case WM_SYSCOMMAND:
			switch (wParam & 0xFFF0)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					if (g_window.fullScreen)
						return FALSE;
					break;

				case SC_KEYMENU:
					return FALSE;
			}
			break;

		case WM_ERASEBKGND:
			return FALSE;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

