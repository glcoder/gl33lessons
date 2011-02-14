#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"

class Window
{
public:
	Window():
		m_width(0), m_height(0), m_fullscreen(false),
		m_active(false), m_focused(false)
	{
	}

	virtual ~Window()
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

	virtual bool initialize()      = 0;
	virtual void clear()           = 0;
	virtual void render()          = 0;
	virtual void update(double dt) = 0;

protected:
	int32_t m_width, m_height;
	bool    m_fullscreen, m_active, m_focused;
};

#endif /* WINDOW_H */
