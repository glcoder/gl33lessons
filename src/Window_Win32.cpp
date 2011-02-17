#include "Window.h"
#include "OpenGL.h"

struct MsgData
{
	UINT   msg;
	WPARAM wParam;
	LPARAM lParam;
};

static const char g_className[] = "OpenGL_window_class";

static HINSTANCE g_hInstance = NULL;
static HWND      g_hWnd      = NULL;
static HDC       g_hDC       = NULL;
static HGLRC     g_hRC       = NULL;
static RECT      g_clipRect;

static uint8_t g_keyStates[0xFF], g_buttons;
static int32_t g_cursorRel[2], g_wheelRel;

static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam);

bool IWindow::create(const char *title, int32_t width, int32_t height,
	bool fullscreen)
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
	RAWINPUTDEVICE        rid[2];

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	g_hInstance = (HINSTANCE)GetModuleHandle(NULL);

	memset(&wcx, 0, sizeof(wcx));
	wcx.cbSize        = sizeof(wcx);
	wcx.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcx.lpfnWndProc   = (WNDPROC)MessageHandler;
	wcx.hInstance     = g_hInstance;
	wcx.lpszClassName = g_className;
	wcx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);

	if (!RegisterClassEx(&wcx))
	{
		LOG_ERROR("RegisterClassEx fail (%d)\n", GetLastError());
		return false;
	}

	style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	exStyle = WS_EX_APPWINDOW;

	x = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
	y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	rect.left   = x;
	rect.right  = x + width;
	rect.top    = y;
	rect.bottom = y + height;

	AdjustWindowRectEx(&rect, style, FALSE, exStyle);

	g_hWnd = CreateWindowEx(exStyle, g_className, title, style,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, g_hInstance, (LPVOID)this);

	if (!g_hWnd)
	{
		LOG_ERROR("CreateWindowEx fail (%d)\n", GetLastError());
		return false;
	}

	g_hDC = GetDC(g_hWnd);

	if (!g_hDC)
	{
		LOG_ERROR("GetDC fail (%d)\n", GetLastError());
		return false;
	}

	memset(g_keyStates, 0, sizeof(g_keyStates));

	g_buttons      = 0;
	g_wheelRel     = 0;
	g_cursorRel[0] = 0;
	g_cursorRel[1] = 0;

	rid[0].usUsagePage = 0x01;
	rid[0].usUsage     = 0x02; // mouse
	rid[0].dwFlags     = 0; //RIDEV_NOLEGACY;
	rid[0].hwndTarget  = g_hWnd;

	rid[1].usUsagePage = 0x01;
	rid[1].usUsage     = 0x06; // keyboard
	rid[1].dwFlags     = 0; //RIDEV_NOLEGACY;
	rid[1].hwndTarget  = g_hWnd;

	if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE)))
	{
		LOG_ERROR("RegisterRawInputDevices fail (%d)\n", GetLastError());
		return false;
	}

	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize      = sizeof(pfd);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;

	format = ChoosePixelFormat(g_hDC, &pfd);
	if (!format || !SetPixelFormat(g_hDC, format, &pfd))
	{
		LOG_ERROR("Setting pixel format fail (%d)\n", GetLastError());
		return false;
	}

	hRCTemp = wglCreateContext(g_hDC);
	if (!hRCTemp || !wglMakeCurrent(g_hDC, hRCTemp))
	{
		LOG_ERROR("Сreating temp render context fail (%d)\n", GetLastError());
		return false;
	}

	GL_GET_PROC_CRITICAL(PFNWGLCREATECONTEXTATTRIBSARBPROC,
		wglCreateContextAttribsARB);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRCTemp);

	g_hRC = wglCreateContextAttribsARB(g_hDC, 0, attribs);
	if (!g_hRC || !wglMakeCurrent(g_hDC, g_hRC))
	{
		LOG_ERROR("Creating render context fail (%d)\n", GetLastError());
		return false;
	}

	GL::information();

	if (!GL::initialize())
		return false;

	GetClipCursor(&g_clipRect);
	ShowCursor(FALSE);
	setSize(width, height, fullscreen);

	return true;
}

void IWindow::destroy()
{
	clear();

	m_active = m_focused = false;

	ClipCursor(&g_clipRect);
	ShowCursor(TRUE);

	if (m_fullscreen)
	{
		ChangeDisplaySettings(NULL, CDS_RESET);
		m_fullscreen = false;
	}

	if (g_hRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_hRC);
		g_hRC = NULL;
	}

	if (g_hDC)
	{
		ReleaseDC(g_hWnd, g_hDC);
		g_hDC = NULL;
	}

	if (g_hWnd)
	{
		DestroyWindow(g_hWnd);
		g_hWnd = NULL;
	}

	if (g_hInstance)
	{
		UnregisterClass(g_className, g_hInstance);
		g_hInstance = NULL;
	}
}

void IWindow::setSize(int32_t width, int32_t height, bool fullscreen)
{
	ASSERT(width > 0);
	ASSERT(height > 0);

	RECT    rect;
	DWORD   style, exStyle;
	DEVMODE devMode;
	LONG    result;
	int     x, y;

	if (m_fullscreen && !fullscreen)
		ChangeDisplaySettings(NULL, CDS_RESET);

	m_fullscreen = fullscreen;

	if (m_fullscreen)
	{
		memset(&devMode, 0, sizeof(devMode));
		devMode.dmSize       = sizeof(devMode);
		devMode.dmPelsWidth  = width;
		devMode.dmPelsHeight = height;
		devMode.dmBitsPerPel = GetDeviceCaps(g_hDC, BITSPIXEL);
		devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

		result = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
		if (result != DISP_CHANGE_SUCCESSFUL)
		{
			LOG_ERROR("ChangeDisplaySettings fail %dx%d (%d)\n",
				width, height, result);

			m_fullscreen = false;
		}
	}

	if (m_fullscreen)
	{
		style   = WS_POPUP;
		exStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;

		x = y = 0;
	} else
	{
		style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		exStyle = WS_EX_APPWINDOW;

		x = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
		y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	}

	rect.left   = x;
	rect.right  = x + width;
	rect.top    = y;
	rect.bottom = y + height;

	AdjustWindowRectEx(&rect, style, FALSE, exStyle);

	SetWindowLong(g_hWnd, GWL_STYLE,   style);
	SetWindowLong(g_hWnd, GWL_EXSTYLE, exStyle);

	SetWindowPos(g_hWnd, HWND_TOP, rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		SWP_FRAMECHANGED);

	SetCursorPos(x + width / 2, y + height / 2);

	ShowWindow(g_hWnd, SW_SHOW);
	SetForegroundWindow(g_hWnd);
	SetFocus(g_hWnd);
	UpdateWindow(g_hWnd);

	ClipCursor(&rect);

	GetClientRect(g_hWnd, &rect);
	m_width  = rect.right - rect.left;
	m_height = rect.bottom - rect.top;

	m_focused = true;
}

void IWindow::setTitle(const char *title)
{
	SetWindowText(g_hWnd, title);
}

int IWindow::mainLoop()
{
	MSG msg;

	double   fpsTime = 0.0;
	uint32_t fps     = 0;

	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
	GL_GET_PROC_CRITICAL(PFNWGLSWAPINTERVALEXTPROC, wglSwapIntervalEXT);

	m_active = initialize();

	wglSwapIntervalEXT(0);

	while (m_active)
	{
		while (PeekMessage(&msg, g_hWnd, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				m_active = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (m_active && m_focused)
		{
			double beginFrameTime = m_timer.getTicks(), frameTime = 0.0;

			render();
			glFinish();
			SwapBuffers(g_hDC);

			frameTime = m_timer.getTicks() - beginFrameTime;

			fpsTime += frameTime;
			++fps;

			if (fpsTime >= 1.0)
			{
				char buffer[100] = "";
				sprintf(buffer, "FPS: %u", fps);
				SetWindowText(g_hWnd, buffer);

				fps     = 0;
				fpsTime = 0.0;
			}

			input(g_cursorRel, g_wheelRel, g_buttons, g_keyStates, frameTime);
			update(frameTime);

			for (uint32_t i = 0; i < sizeof(g_keyStates); ++i)
				if (g_keyStates[i] == INPUT_KEY_PRESSED)
					g_keyStates[i] = INPUT_KEY_DOWN;
		} else
		{
			Sleep(2);
		}

		g_cursorRel[0] = g_cursorRel[1] = 0;
		g_wheelRel = 0;
	}

	m_active = m_focused = false;
	return 0;
}

template <> int IWindow::messageHandler(const MsgData &data)
{
	switch (data.msg)
	{
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		{
			m_focused = (data.msg == WM_SETFOCUS);
			return FALSE;
		}

		case WM_ACTIVATE:
		{
			m_focused = (LOWORD(data.wParam) != WA_INACTIVE);
			return FALSE;
		}

		case WM_CLOSE:
		{
			m_active = m_focused = false;
			PostQuitMessage(0);
			return FALSE;
		}

		case WM_SYSCOMMAND:
		{
			switch (data.wParam & 0xFFF0)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				{
					if (m_fullscreen)
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

		case WM_INPUT:
		{
			if (!m_focused)
				break;

			static RAWINPUT raw;
			UINT size = sizeof(raw);

			if (GetRawInputData((HRAWINPUT)data.lParam, RID_INPUT, (PBYTE)&raw,
				&size, sizeof(RAWINPUTHEADER)) == (UINT)(-1))
			{
				LOG_ERROR("Fail to get raw data from GetRawInputData\n");
				return FALSE;
			}

			if (raw.header.dwType == RIM_TYPEKEYBOARD
				&& raw.data.keyboard.VKey < 0xFF)
			{
				uint8_t state = g_keyStates[raw.data.keyboard.VKey];

				if (raw.data.keyboard.Flags == RI_KEY_MAKE)
				{
					if (state == INPUT_KEY_PRESSED)
						state = INPUT_KEY_DOWN;
					else if (state == INPUT_KEY_UP)
						state = INPUT_KEY_PRESSED;
				} else if (raw.data.keyboard.Flags == RI_KEY_BREAK)
				{
					state = INPUT_KEY_UP;
				}

				g_keyStates[raw.data.keyboard.VKey] = state;
			} else if (raw.header.dwType == RIM_TYPEMOUSE) 
			{
				if (raw.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
					g_buttons |= INPUT_BUTTON_LEFT;
				if (raw.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
					g_buttons |= INPUT_BUTTON_RIGHT;
				if (raw.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
					g_buttons |= INPUT_BUTTON_MIDDLE;

				if (raw.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
					g_buttons &= ~INPUT_BUTTON_LEFT;
				if (raw.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
					g_buttons &= ~INPUT_BUTTON_RIGHT;
				if (raw.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
					g_buttons &= ~INPUT_BUTTON_MIDDLE;

				if (raw.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
					g_wheelRel += (int32_t)raw.data.mouse.usButtonData;

				if (raw.data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
				{
					g_cursorRel[0] += raw.data.mouse.lLastX;
					g_cursorRel[1] += raw.data.mouse.lLastY;
				}
			}

			return FALSE;
		}
	}

	return DefWindowProc(g_hWnd, data.msg, data.wParam, data.lParam);
}

LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	static IWindow *window = NULL;
	MsgData        data    = {msg, wParam, lParam};

	if (msg == WM_CREATE)
	{
		CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
		window = (IWindow *)cs->lpCreateParams;
	}

	return window ? (LRESULT)window->messageHandler(data)
		: DefWindowProc(hWnd, msg, wParam, lParam);
}
