#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"
#include "Timer.h"

#define INPUT_KEY_UP      0
#define INPUT_KEY_PRESSED 1
#define INPUT_KEY_DOWN    2

#define INPUT_BUTTON_LEFT   0x01
#define INPUT_BUTTON_RIGHT  0x02
#define INPUT_BUTTON_MIDDLE 0x04

class IWindow
{
public:
	IWindow():
		m_width(0), m_height(0), m_fullscreen(false),
		m_active(false), m_focused(false), m_timer()
	{
	}

	virtual ~IWindow()
	{
		if (m_active)
			destroy();

		m_active = false;
	}

	bool create(const char *title, int32_t width, int32_t height,
		bool fullscreen = false);

	void destroy();

	void setSize(int32_t width, int32_t height, bool fullscreen = false);
	void setTitle(const char *title);

	int mainLoop();

	template <typename MSGDATA> int messageHandler(const MSGDATA &data);

	int32_t getWidth()  const { return m_width;  }
	int32_t getHeight() const { return m_height; }

	bool ifFullscreen() const { return m_fullscreen; }
	bool isActive()     const { return m_active;     }
	bool isFocused()    const { return m_focused;    }

	virtual bool initialize()             = 0;
	virtual void clear()                  = 0;
	virtual void render()                 = 0;
	virtual void update(double frameTime) = 0;

	virtual void input(const int32_t *cursorRel, int32_t wheelRel,
		uint8_t buttons, const uint8_t *keyStates, double frameTime) = 0;

protected:
	int32_t m_width, m_height;
	bool    m_fullscreen, m_active, m_focused;
	Timer   m_timer;
};

#endif /* WINDOW_H */
