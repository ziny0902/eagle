#pragma once
#include <memory>
#include <GLPP/glBuffer.h>
#include <GLPP/shader.h>
#include <GLPP/vertex_arrays.h>
#include <Geo/geometry_3d_object.h>
#include <util/ini.h>

namespace Gl
{

class GlWidget{
public:
  GlWidget(
    int x,
    int y,
    int w,
    int h
  );
  GlWidget();

  // virtual function declear.
  virtual ~GlWidget();
  virtual void update(){}
  virtual void on_drag(const int x, const int y);
  virtual void on_mouse_button();
  virtual void resource_release(int) {}

  void move(const int x, const int y);
  void resize(const int w, const int h);
  glm::vec4 get_widget_fg();
  glm::vec4 get_widget_bg();
  void get_position(int& x, int& y);
  void get_size(int& w, int& h);
  char *get_vertex_bg_data();
  char *get_vertex_fg_data();
  int get_vertex_data_bg_bytes();
  int get_vertex_data_fg_bytes();
  void set_widget_bg(glm::vec4& color);
  void set_widget_fg(glm::vec4& color);
  inline bool is_realized() {return m_is_realize;}
  inline bool is_hide() { return m_is_hide; }
  virtual void set_window_size(int w, int h)
  {
    m_ww = w;
    m_wh = h;
  }
  inline void set_parent(GlWidget *parent)
  {
    m_parent = parent;
  }
  inline void set_id(int id)
  {
    m_id = id;
  }
protected:
  inline int get_id()
  {
    return m_id;
  }

  // size, position of widget
  int m_ww, m_wh;
  int m_x;
  int m_y;
  int m_width;
  int m_height;
  glm::vec4 m_fg;
  glm::vec4 m_bg;

  // geo::GeoModel3D m_bg_data;
  // geo::GeoModel3D m_fg_data;
  std::vector<float> m_bg_data; 
  std::vector<unsigned short> m_fg_data; 

  GlWidget *m_parent;
  int m_id;
  bool m_is_realize;
  bool m_is_hide;
};

}
