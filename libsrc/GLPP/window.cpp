#include <GLPP/window.h>
#include <iostream>

using namespace Gl;

void mouse_button_callback_wrap(GLFWwindow *glfw_w, int button, int action, int mods)
{ 
  Window * w = (Window *)glfwGetWindowUserPointer(glfw_w);
  w->mouse_button_callback(glfw_w, button, action, mods); 
}

void window_size_callback_wrap(GLFWwindow *glfw_w, int height, int width)
{ 
  Window * w = (Window *)glfwGetWindowUserPointer(glfw_w);
  w->window_size_callback(glfw_w, height, width); 
}

bool Window::glfwInitialize(void)
{
  // Initialise GLFW
  glewExperimental = true; // Needed for core profile

  if( !glfwInit() )
  {
    std::cerr << "Failed to initialize GLFW\n";
    return false;
  }

  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

  m_window = glfwCreateWindow( m_w, m_h, m_title.c_str(), NULL, NULL);
  if( m_window == NULL ){
    std::cerr << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n";
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(m_window); // Initialize GLEW
  glewExperimental=true; // Needed in core profile
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW\n";
    return false;
  }
  glfwSetWindowUserPointer(m_window, (void *) this);

  // register event handler
  glfwSetWindowSizeCallback(m_window, window_size_callback_wrap);
  glfwSetMouseButtonCallback(m_window, mouse_button_callback_wrap); 

  return true;
}

void Window::update(void)
{
			glfwSwapBuffers(m_window);
}

void Window::run(void)
{
  // if ( glfwInitialize() == false)
  // {
  //   std::cerr << "glfwInitialize() fail\n";
  //   return;
  // }
  if( m_interface != nullptr) {
    m_interface->StartUp();
  }

  do{

    if( m_interface != nullptr) {
      m_interface->Draw(this);

      glfwSwapBuffers(m_window);
    }

    glfwPollEvents();

  }while(glfwWindowShouldClose(m_window) == 0);

  std::cout << "glfw Window Close()\n";
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if(m_interface != nullptr)
    m_interface->MouseButton(button, action, mods);
}

void Window::window_size_callback(GLFWwindow *window, int height, int width)
{
  if(m_interface != nullptr)
    m_interface->WindowResize(height, width);
}
