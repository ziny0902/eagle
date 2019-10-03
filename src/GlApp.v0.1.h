#ifndef __GL_APP_3D_H__
#define __GL_APP_3D_H__
#include <vector>
#include <GLPP/texture.h>
#include <util/ini.h>
#include "axis.3D.h"
#include "mesh.h"
#include "plot3d.h"
#include "vector3d.h"
#include "object3d.h"
#include <GLPP/gl_resource_manager.h>
#include "glWidgetWindow.h"

#define MAX_VAO_NUM   2

class GlApp3D {

public:
  GlApp3D(IniManager&);
  ~GlApp3D();
  virtual void Draw();
  virtual void StartUp(IniManager&);
  void set_window_size(int w, int h);
  void ChangeLookAt(float x, float y, float z);
  void ModelRotate(float x, float y, float z);
  void ModelTranslate(float x, float y, float z);
  void screen_2_world(float &x, float &y, float &z);
  void display_pixel_info(int x, int y);
  void mouse_release(int x, int y);
  void add_vector(glm::vec3 s, glm::vec3 e);
  inline bool is_selected() {
    return is_object_selected;
  }
  void del_select_object();

private:
  int m_w, m_h;
  bool m_mesh_enabled, m_plot3d_enabled, m_figure_enabled, m_vector_enabled;
  bool is_object_selected;
  const std::string c_u_color = "u_Color";

  unsigned int m_vertArray;
  unsigned int m_2d_vertArray;
  unsigned int m_shader;
  Gl::ResourceManager m_manager;

  Axis3D m_x_axis;
  Axis3D m_y_axis;
  Axis3D m_z_axis;

  Plot3D m_plot3d;
  Mesh m_mesh;
  Vector3d m_vector3d;
  Object3D m_figure;
  glWidgetWindow m_gl_window;

  glm::mat4 m_rotate;
  glm::mat4 m_projection;
  glm::mat4 m_view;
  glm::mat4 m_model;

  void post_pixel_sel
    (std::string &s, float x, float y, float& z, unsigned int);

};

#endif
