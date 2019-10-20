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
    bool ret =
    m_highlight.get_selected_obj_type() == app_common::app_gl_object::none
        ? false : true;
    return ret;
  }
  app_common::app_gl_object get_selected_obj_type(){
    return m_highlight.get_selected_obj_type();
  }
  unsigned short get_enabled_object()
  { return m_obj_enabled; }

  void del_select_object();
  void add_TNB_frame(float t
                     , unsigned short sel_vector
                     , unsigned short sel_plane
                     , glm::vec4 plane_color
                     );
  float get_plot3d_selected_parameter();

private:
  int m_w, m_h;
  unsigned short m_obj_enabled;

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
