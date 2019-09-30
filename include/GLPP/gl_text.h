#pragma once
#include <vector>
#include <string>
#include <util/ini.h>
#include <GLPP/glFont.h>
#include "gl_drawing_obj.h"
#include <ft2build.h>
#include FT_FREETYPE_H

//
// inheritance from parent(Gl::Drawable_obj)
//
// protected:
// gl_resource_id 
//
// publick:
// virtual Update()
// virtual init_gl_buffer()
//
class GlTextObject : Gl::Drawable_obj{
public:
  GlTextObject(std::string &&s);
  GlTextObject(std::string &&s, IniManager& ini);
  void update_string
  (
    std::string &&s, 
    Gl::ResourceManager& manager
  );
  int get_width();
  int get_height();
  void set_pos(int x, int y, Gl::ResourceManager& manager);
  void set_scale(float scale);
  void set_font_size(size_t size);

  // for othogonal projection.
  void set_window_size(
    int w, 
    int h, 
    Gl::ResourceManager &manager 
  );

  // Instance of virtual function.

  void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager);
  void init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id
  );

private:
  /* GL Resource start*/
  int m_shader;
  /* GL Resource end.*/
  
  int m_sx, m_sy; //text starting coordinate in pixels
  int m_ww, m_wh; // window size.
  float m_scale;
  int m_f_size; // font size
  int m_tw, m_th; //text size in pixels
  std::string m_text;
  std::vector<float> m_points;
  std::shared_ptr<std::vector<double>> m_color;

  Gl::GlCharacterTable m_ch_font;
  void init_vertex_data();
  void init();

};
