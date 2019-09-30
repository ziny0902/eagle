#include <GLPP/glWidget.h>

using namespace Gl;

#define APPEND(v, p) \
  v.push_back(p.x);\
  v.push_back(p.y);\
  v.push_back(p.z);


GlWidget::GlWidget() :
   m_fg(1.0, 1.0, 1.0, 1.0)
  ,m_bg(1.0, 1.0, 1.0, 0.5)
{
  m_fg = glm::vec4(1);
  m_bg = glm::vec4(0, 0, 0, 0.5);
  m_x = 0;
  m_y = 0;
  m_width = 0;
  m_height = 0;
  m_parent = nullptr;
  m_id = -1;
  m_is_realize = false;
  m_is_hide = false;
}

GlWidget::GlWidget(
  int x,
  int y,
  int w,
  int h
) :
   m_fg(1.0, 1.0, 1.0, 1.0)
  ,m_bg(1.0, 1.0, 1.0, 0.5)
{
  m_x = x;
  m_y = y;

  resize(w, h);

  m_is_realize = false;
  m_is_hide = false;
  m_parent = nullptr;
  m_id = -1;
}

GlWidget::~GlWidget()
{
  if(m_parent != nullptr) 
  {
    m_parent->resource_release(m_id);
    m_parent = nullptr;
    m_id = -1;
  }
}

char *GlWidget::get_vertex_bg_data()
{
  // return (char *)m_bg_data.getArray();
  return (char *)&m_bg_data[0];
}

char *GlWidget::get_vertex_fg_data()
{
  // return (char *)m_fg_data.getArray();
  return (char *)&m_fg_data[0];
}

int GlWidget::get_vertex_data_bg_bytes()
{
  return m_bg_data.size()*sizeof(float);
}

int GlWidget::get_vertex_data_fg_bytes()
{
  return m_fg_data.size()*sizeof(float);
}

void GlWidget::move(const int x, const int y)
{
  m_x = x;
  m_y = y;
}

void GlWidget::resize(const int w, const int h)
{
  m_width = w;
  m_height = h;

    // 
    //               
    //  (0, h) |-----| (w, h)
    //         |     |
    //  (0, 0) |-----| (w, 0)
    //
std::cout << m_width << " "  << m_height << std::endl;
  m_bg_data.clear();
  
  APPEND(m_bg_data, glm::vec3(0, h, 0));
  APPEND(m_bg_data, glm::vec3(0, 0, 0));
  APPEND(m_bg_data, glm::vec3(w, 0, 0));

  APPEND(m_bg_data, glm::vec3(0, h,  0));
  APPEND(m_bg_data, glm::vec3(w,  0, 0));
  APPEND(m_bg_data, glm::vec3(w, h, 0));

  m_fg_data.clear();
#ifdef __DEBUG__
  m_fg_data.push_back(1);
  m_fg_data.push_back(0);
  m_fg_data.push_back(5);
  m_fg_data.push_back(2);
#endif
  APPEND(m_fg_data, glm::vec3(0, 0, 0));
  APPEND(m_fg_data, glm::vec3(0, h, 0));
  APPEND(m_fg_data, glm::vec3(w, h, 0));
  APPEND(m_fg_data, glm::vec3(w, 0, 0));

}

void GlWidget::set_widget_bg(glm::vec4& color)
{
  m_bg = color;
}

void GlWidget::set_widget_fg(glm::vec4& color)
{
  m_fg = color;
}

glm::vec4 GlWidget::get_widget_bg()
{
  return m_bg;
}

glm::vec4 GlWidget::get_widget_fg()
{
  return m_fg;
}

void GlWidget::get_position(int& x, int& y)
{
  x = m_x;
  y = m_y;
}

void GlWidget::get_size(int& w, int& h)
{
  w = m_width;
  h = m_height;
}


void GlWidget::on_drag(const int x, const int y)
{
}

void GlWidget::on_mouse_button()
{
}
