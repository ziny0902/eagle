#pragma once
#include <GLPP/gl_drawing_obj.h>
#include "define_common.h" 

class Highlight : public Gl::Drawable_obj{
 public:
  Highlight();
  ~Highlight();

  // Instance of virtual function.
  void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager);
  void init_gl_buffer(
      Gl::ResourceManager &manager,
      Gl::VertexBufferLayout& layout,
      int shader_id
                      );
  //
  void set_highlight(
      Gl::ResourceManager& manager
      , unsigned char *vertices
      , int bytes
      , unsigned int sel_id
      , unsigned int sub_id
      , app_common::app_gl_object sel_obj
                     );
  void set_selected_coord(Gl::ResourceManager& manager
                          , glm::vec3& coord);
  void set_selected_coord(Gl::ResourceManager& manager
                          , glm::vec3&& coord);
  glm::vec3& get_selected_coord()
  { return m_coord; }
  app_common::app_gl_object get_selected_obj_type()
  { return m_selected_object; }

  void clear();
  unsigned int get_oid() { return m_selected_oid; }

 private:
  glm::vec4 m_color;
  glm::vec3 m_coord;
  app_common::app_gl_object m_selected_object ;
  unsigned int m_selected_oid;
  unsigned int m_marker_resource;
  unsigned int m_sub_id;
  bool is_delegated;
};
