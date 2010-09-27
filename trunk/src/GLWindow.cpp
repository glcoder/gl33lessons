#include "GLWindow.h"

GLWindow::GLWindow(): m_hInstance(NULL), m_hWnd(NULL), m_hDC(NULL), m_hRC(NULL),
	m_width(0), m_height(0), m_fullScreen(false), m_active(false), m_running(false),
	m_renderProc(NULL)
{
}

GLWindow::~GLWindow()
{
	// восстановим разрешение экрана
	if (m_fullScreen)
	{
		ChangeDisplaySettings(NULL, CDS_RESET);
		ShowCursor(TRUE);
	}

	// удаляем контекст рендеринга
	if (m_hRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hRC);
	}

	// освобождаем контекст окна
	if (m_hDC)
		ReleaseDC(m_hWnd, m_hDC);

	// удаляем окно
	if (m_hWnd)
		DestroyWindow(m_hWnd);

	// удаляем класс окна
	if (m_hInstance)
		UnregisterClass(GLWINDOW_CLASS_NAME, m_hInstance);
}

bool GLWindow::create(const char *title, int width, int height, bool fullScreen)
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

	m_hInstance = static_cast<HINSTANCE>(GetModuleHandle(NULL));

	// регистрация класса окна
	memset(&wcx, 0, sizeof(wcx));
	wcx.cbSize        = sizeof(wcx);
	wcx.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcx.lpfnWndProc   = reinterpret_cast<WNDPROC>(GLWindow::sWindowProc);
	wcx.hInstance     = m_hInstance;
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
	m_hWnd = CreateWindowEx(exStyle, GLWINDOW_CLASS_NAME, title, style, rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, m_hInstance, static_cast<LPVOID>(this));

	if (!m_hWnd)
	{
		LOG_ERROR("CreateWindowEx fail (%d)\n", GetLastError());
		return false;
	}

	// получим дескриптор контекста окна
	m_hDC = GetDC(m_hWnd);

	if (!m_hDC)
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
	format = ChoosePixelFormat(m_hDC, &pfd);
	if (!format || !SetPixelFormat(m_hDC, format, &pfd))
	{
		LOG_ERROR("Setting pixel format fail (%d)\n", GetLastError());
		return false;
	}

	// создадим временный контекст рендеринга
	// он нужен для получения функции wglCreateContextAttribsARB
	hRCTemp = wglCreateContext(m_hDC);
	if (!hRCTemp || !wglMakeCurrent(m_hDC, hRCTemp))
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

	// создадим контекст рендеринга для OpenGL 3.x
	m_hRC = wglCreateContextAttribsARB(m_hDC, 0, attribs);
	if (!m_hRC || !wglMakeCurrent(m_hDC, m_hRC))
	{
		LOG_ERROR("Creating render context fail (%d)\n", GetLastError());
		wglDeleteContext(hRCTemp);
		return false;
	}

	// выведем в лог немного информации о контексте рендеринга
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

	// больше нам временный контекст рендеринга не нужен
	wglDeleteContext(hRCTemp);

	// запишем указатель на себя в дескриптор окна для корректной работы windowProc
	SetProp(m_hWnd, GLWINDOW_PROP_NAME, reinterpret_cast<HANDLE>(this));

	// зададим размеры окна
	setSize(width, height, fullScreen);

	m_active = m_running = true;

	return true;
}

void GLWindow::setSize(int width, int height, bool fullScreen)
{
	ASSERT(width > 0);
	ASSERT(height > 0);

	RECT    rect;
	DWORD   style, exStyle;
	DEVMODE devMode;
	LONG    result;
	int     x, y;

	// если мы возвращаемся из полноэкранного режима
	if (m_fullScreen && !fullScreen)
	{
		ChangeDisplaySettings(NULL, CDS_RESET);
		ShowCursor(TRUE);
	}

	m_fullScreen = fullScreen;

	// если необходим полноэкранный режим
	if (m_fullScreen)
	{
		memset(&devMode, 0, sizeof(devMode));
		devMode.dmSize       = sizeof(devMode);
		devMode.dmPelsWidth  = width;
		devMode.dmPelsHeight = height;
		devMode.dmBitsPerPel = GetDeviceCaps(m_hDC, BITSPIXEL);
		devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

		// попытка установить полноэкранный режим
		result = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
		if (result != DISP_CHANGE_SUCCESSFUL)
		{
			LOG_ERROR("ChangeDisplaySettings fail %dx%d (%d)\n", width, height, result);
			m_fullScreen = false;
		}
	}

	// если был запрошен полноэкранный режим и его удалось установить
	if (m_fullScreen)
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
	SetWindowLong(m_hWnd, GWL_STYLE,   style);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, exStyle);

	// обновим позицию окна
	SetWindowPos(m_hWnd, HWND_TOP, rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		SWP_FRAMECHANGED);

	// покажем окно на экране
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
	UpdateWindow(m_hWnd);

	// получим размеры окна
	GetClientRect(m_hWnd, &rect);
	m_width  = rect.right - rect.left;
	m_height = rect.bottom - rect.top;

	// устанавливаем вьюпорт на все окно
	OPENGL_CALL(glViewport(0, 0, m_width, m_height));

	// центрируем курсор относительно окна
	SetCursorPos(x + m_width / 2, y + m_height / 2);

	OPENGL_CHECK_FOR_ERRORS();
}

void GLWindow::setRenderProc(pRenderProc renderProc)
{
	m_renderProc = renderProc;
}

int GLWindow::mainLoop()
{
	MSG msg;

	// основной цикл окна
	m_running = m_active = true;
	while (m_running)
	{
		// обработаем сообщения из очереди сообщений
		while (PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				m_running = false;
				break;
			}
			// TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// если окно в рабочем режиме и активно
		if (m_running && m_active)
		{
			if (m_renderProc)
				m_renderProc(this);

			SwapBuffers(m_hDC);
		}

		//Sleep(2);
	}

	m_running = m_active = false;
	return 0;
}

LRESULT CALLBACK GLWindow::windowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				m_running = false;

			if (wParam == VK_F1)
				setSize(m_width, m_height, !m_fullScreen);

			return FALSE;

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			m_active = (msg == WM_SETFOCUS);
			return FALSE;

		case WM_ACTIVATE:
			m_active = (LOWORD(wParam) == WA_INACTIVE);
			return FALSE;

		case WM_CLOSE:
			m_running = m_active = false;
			PostQuitMessage(0);
			return FALSE;

		case WM_SYSCOMMAND:
			switch (wParam & 0xFFF0)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					if (m_fullScreen)
						return FALSE;
					break;

				case SC_KEYMENU:
					return FALSE;
			}
			break;

		case WM_ERASEBKGND:
			return FALSE;
	}

	return DefWindowProc(m_hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK GLWindow::sWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GLWindow *window = reinterpret_cast<GLWindow*>(GetProp(hWnd, GLWINDOW_PROP_NAME));

	return window ? window->windowProc(msg, wParam, lParam) : DefWindowProc(hWnd, msg, wParam, lParam);
}
