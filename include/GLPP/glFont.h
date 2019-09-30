#pragma once
#include <string>
#include <epoxy/gl.h>
#include <util/ini.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Gl
{
#define MAX_FONT_TEXTURE_WIDTH 1024

struct glyph_info_t {
    float ax; // advance.x
    float ay; // advance.y

    float bw; // bitmap.width;
    float bh; // bitmap.height;

    float bl; // bitmap_left;
    float bt; // bitmap_top;

    float tx; // x offset of glyph in texture coordinates
    float ty; // y offset of glyph in texture coordinates
};

class GlCharacterTable{
public:
  GlCharacterTable(IniManager& ini);
  GlCharacterTable(){}

  inline int get_lineheight() { return m_line_height; }

  inline glyph_info_t* const get_glyph_info(uint8_t c) {
    return &m_glyph[c];
  }

  inline void get_texture_size(int& w, int& h)
  { w = m_tex_w; h = m_tex_h;}

  inline GLuint get_texture_name(){ return m_tex_resource; }

private:
  GLuint m_tex_resource; // texture gl object

  /**
    glyph infomation variable 
  **/
  int m_tex_w; // width of texture in pixels
  int m_tex_h; // height of texture in pixels
  int m_line_height;

  glyph_info_t m_glyph[128];

  // private method
  bool load_freetype_info(
    size_t size
    , std::string& f_name
  );
  bool calculate_texture_wh(FT_Face face);
};
}
