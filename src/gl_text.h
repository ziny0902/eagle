#pragma once
#include <vector>
#include <string>
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
  void update_string
  (
    std::string &&s, 
    Gl::ResourceManager& manager
  );
  int get_width();
  int get_height();
  void set_pos(int x, int y);
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
  GLuint m_tex_resource; // texture gl object
  int m_shader;
  unsigned int m_U_tex_id; // fragment shader uniform value tex.
  /* GL Resource end.*/
  
  int m_sx, m_sy; //text starting coordinate in pixels
  int m_ww, m_wh; // window size.
  int m_scale;
  int m_f_size; // font size
	int m_tw, m_th; //text size in pixels
  std::string m_text;
  std::vector<float> m_points;

  /**
    glyph infomation variable 
  **/
  int m_tex_w; // width of texture in pixels
  int m_tex_h; // height of texture in pixels

  struct {
    float ax;	// advance.x
    float ay;	// advance.y

    float bw;	// bitmap.width;
    float bh;	// bitmap.height;

    float bl;	// bitmap_left;
    float bt;	// bitmap_top;

    float tx;	// x offset of glyph in texture coordinates
    float ty;	// y offset of glyph in texture coordinates
  } m_glyph[128];

  bool load_freetype_info(
    size_t size
  );
  bool calculate_texture_wh(FT_Face face);
  void init_vertex_data();
};
