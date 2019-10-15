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
#include "plane3d.h"
#include "Highlight.h"

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
  void add_plane(
      glm::vec3& s
      , glm::vec3& e
      , glm::vec3& c
      , double w
      , double h
      , glm::vec4& color
                 );
  inline bool is_selected() {
    return is_object_selected;
  }
  void del_select_object();
  void add_curvature_info(float t);

private:
  int m_w, m_h;
  bool m_mesh_enabled, m_plot3d_enabled, m_figure_enabled, m_vector_enabled;
  bool is_object_selected;

  unsigned int m_shader;
  unsigned int m_mesh_shader;
  int m_uniBuffer_id;
  Gl::ResourceManager m_manager;
  Gl::GlBuffer m_uniBuffer;

  Axis3D m_x_axis;
  Axis3D m_y_axis;
  Axis3D m_z_axis;

  Plot3D m_plot3d;
  Mesh m_mesh;
  Vector3d m_vector3d;
  Object3D m_figure;
  glWidgetWindow m_gl_window;
  Plane3d m_plane3d;
  Highlight m_highlight;

  glm::mat4 m_rotate;
  glm::mat4 m_projection;
  glm::mat4 m_view;
  glm::mat4 m_model;

  void post_pixel_sel
    (std::string &s, float x, float y, float& z, unsigned int);

};

#endif
