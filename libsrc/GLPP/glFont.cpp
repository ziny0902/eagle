#include <iostream>
#include <memory>
#include <GLPP/util.h>
#include <util/font_util.h>
#include <GLPP/glFont.h>

using namespace Gl;

static void bitmap_dump(unsigned char *bitmap, int w, int h)
{
  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++) {
      printf("%02x ", *(bitmap+(i*w)+j));
    }
    printf("\n");
  }
}

bool GlCharacterTable::calculate_texture_wh(FT_Face face)
{
  FT_GlyphSlot g = face->glyph;

  int roww = 0; // maximum glyph texture width
  int rowh = 0; // maximum glyph texture height

  m_tex_w = 0;
  m_tex_h = 0;
  m_line_height = 0;
  memset(m_glyph, 0, sizeof m_glyph);
  for(int i = 32; i < 128; i++) {
    if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
      fprintf(stderr, "Loading character %c failed!\n", i);
      continue;
    }
    if (roww + g->bitmap.width + 1 >= MAX_FONT_TEXTURE_WIDTH) {
      m_tex_w = std::max(m_tex_w, roww);
      m_tex_h += rowh;
      roww = 0;
      rowh = 0;
    }
    roww += g->bitmap.width + 1;
    rowh = std::max(rowh, (int)g->bitmap.rows);
    m_line_height = std::max(m_line_height, (int)g->bitmap.rows);
  }

  m_tex_w = std::max(m_tex_w, roww);
  m_tex_h += rowh;
#ifdef __DEBUG__
std::cout << "texture w:" << m_tex_w << std::endl;
std::cout << "texture h:" << m_tex_h << std::endl;
#endif
  return true;
}

bool GlCharacterTable::load_freetype_info(
  size_t size
  , std::string& f_name
){
  FT_Library ft;
  if (FT_Init_FreeType(&ft)){
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    return false;
  }

  FT_Face face = load_face_from_fontname(ft, f_name.c_str());
  if (face == NULL){
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl; 
    FT_Done_FreeType(ft);
    return false;
  }

  FT_Set_Pixel_Sizes(face, 0, size);  
  calculate_texture_wh(face);

  /* Create a texture that will be used to hold all ASCII glyphs */
  GLCall(glGenTextures(1, &m_tex_resource));
  GLCall(glBindTexture(GL_TEXTURE_2D, m_tex_resource));

  /* We require 1 byte alignment when uploading texture data */
  GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    GLCall(glTexImage2D(
    GL_TEXTURE_2D, 
    0, 
    GL_RED, 
    m_tex_w, 
    m_tex_h, 
    0, 
    GL_RED, 
    GL_UNSIGNED_BYTE, 
    0
  ));

  /* Clamping to edges is important to prevent artifacts when scaling */
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

  /* Linear filtering usually looks best for text */
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

  /* Paste all glyph bitmaps into the single texture, remembering the offset */
  int ox = 0;
  int oy = 0;

  int rowh = 0;

  for (int i = 32; i < 128; i++) {
    if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
      fprintf(stderr, "Loading character %c failed!\n", i);
      continue;
    }
    FT_GlyphSlot g = face->glyph;

    if (ox + g->bitmap.width + 1 >= MAX_FONT_TEXTURE_WIDTH) {
      oy += rowh;
      rowh = 0;
      ox = 0;
    }

    GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer));
#ifdef __DEBUG__
    bitmap_dump(g->bitmap.buffer, g->bitmap.width, g->bitmap.rows);
#endif
    /* he horizontal advance e.g. the horizontal distance (in 1/64th pixels) 
     * from the origin to the origin of the next glyph */
    m_glyph[i].ax = g->advance.x >> 6; // Bitshift by 6 to get value in pixels (2^6 = 64)
    m_glyph[i].ay = g->advance.y >> 6;

    m_glyph[i].bw = g->bitmap.width;
    m_glyph[i].bh = g->bitmap.rows;

    m_glyph[i].bl = g->bitmap_left;
    m_glyph[i].bt = g->bitmap_top;

    m_glyph[i].tx = (float)ox / (float)m_tex_w;
    m_glyph[i].ty = (float)oy / (float)m_tex_h;

    rowh = std::max(rowh, (int)g->bitmap.rows);
    ox += g->bitmap.width + 1;
  }

  GLCall(glBindTexture(GL_TEXTURE_2D, 0));
  // Destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
  return true;
}

GlCharacterTable::GlCharacterTable(IniManager& ini)
{
  size_t f_size 
    = (size_t)ini.get_integer("FONT", "size");
  std::shared_ptr<std::string> f_name 
    = ini.get_string("FONT", "name");
  
  bool ret = load_freetype_info(f_size, *f_name);
  if(ret == false) {
    std::cout << __FILE__ << "(" << __LINE__ << ")" <<
    "GlCharacterTable() fail." << std::endl;
    //TODO need to raise signal.
    return;
  }
}
