#ifndef __GL_WINDOW_H__
#define __GL_WINDOW_H__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <GLPP/util.h>

namespace Gl
{

class WinInterface;

class Window {
#define DEFAULT_H	600
#define DEFAULT_W	600
public:
	Window() 
	{
		m_window = nullptr;
		m_title = "GL Window";
		// default window size 600 x 600
		m_w = DEFAULT_H;
		m_h = DEFAULT_W;
    m_interface = nullptr;
    ASSERT(glfwInitialize())
	}

	Window (std::string &&title, unsigned int w = DEFAULT_H, unsigned int h = DEFAULT_H) 
	{
		m_window = nullptr;
		m_title = title;
		m_w = w;
		m_h = h;
    m_interface = nullptr;
    ASSERT(glfwInitialize())
	}

  ~Window()
  {
    glfwTerminate();
  }

	// void setInterface(WinInterface *interface)
	// {
	// 	m_interface = interface;
	// }
	void setInterface(std::shared_ptr<WinInterface> interface)
	{
		m_interface = interface;
	}


  void update(void);
	void run(void);

  void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
  void window_size_callback(GLFWwindow *window, int height, int width);

private:
	std::string m_title;
	GLFWwindow *m_window;
	unsigned int m_w, m_h;
	std::shared_ptr<WinInterface> m_interface;
	bool glfwInitialize(void);
};

class WinInterface {
public :
	virtual void Draw(Window *win) {}
  virtual void StartUp(void) {}
  virtual void MouseButton(int button, int action, int mods) {}
  virtual void WindowResize(int height, int width) {}
};


}

#endif
