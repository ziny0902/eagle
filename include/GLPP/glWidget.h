#pragma once
#include <memory>
#include <GLPP/glBuffer.h>
#include <GLPP/shader.h>
#include <GLPP/vertex_arrays.h>
#include <Geo/geometry_3d_object.h>
#include <util/ini.h>

class GlWidget{
public:
  GlWidget(
    std::shared_ptr<Gl::Shader> shader, 
    IniManager& ini,
    int x,
    int y,
    int w,
    int h
  );

  // virtual function declear.
  virtual ~GlWidget();
  virtual void Update();
  virtual void Drag(const int x, const int y);
  virtual void Clicked();

  void set_window_pos(const int x, const int y);
  void set_window_size(const int w, const int h);
  void set_window_bg(glm::vec4& color);
  void get_window_pos(int& x, int& y);
  void get_window_size(int& w, int& h);

private:
  // Gl Resources
  std::shared_ptr<Gl::Shader> m_shader;
  Gl::GlBuffer m_vao;
  Gl::VertexArrays m_va;
  // size, position of widget
  int m_x;
  int m_y;
  int m_width;
  int m_height;
  glm::vec4 m_color;

  geo::GeoModel3D data;
};
