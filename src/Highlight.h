#pragma once
#include <GLPP/gl_drawing_obj.h>

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
                     );
  void clear();
  unsigned int get_oid() { return m_selected_oid; }

 private:
  glm::vec4 m_color;
  unsigned int m_selected_oid;
  unsigned int m_sub_id;
  bool is_delegated;
};
