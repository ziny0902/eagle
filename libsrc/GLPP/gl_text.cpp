#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <GLPP/gl_text.h>
#include <util/font_util.h>
#include <GLPP/util.h>

#define DEFAULT_FONT_SIZE 30 

// Maximum texture width
#define MAXWIDTH 1024

bool GlTextObject::calculate_texture_wh(FT_Face face)
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
    if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
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


static void bitmap_dump(unsigned char *bitmap, int w, int h)
{
  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++) {
      printf("%02x ", *(bitmap+(i*w)+j));
    }
    printf("\n");
  }
}


#include <fontconfig/fontconfig.h>
bool GlTextObject::load_freetype_info(
  size_t size
)
{
  FT_Library ft;
  if (FT_Init_FreeType(&ft)){
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    return false;
  }

  FT_Face face = load_face_from_fontname(ft, "D2Cording");
  if (face == NULL){
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl; 
    FT_Done_FreeType(ft);
    return false;
  }

  FT_Set_Pixel_Sizes(face, 0, size);  
  calculate_texture_wh(face);

  /* Create a texture that will be used to hold all ASCII glyphs */
  GLCall(glActiveTexture(GL_TEXTURE0));
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

    if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
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

void GlTextObject::init_vertex_data()
{
  float x = 0;
  float y = 0;
  /* Loop through all characters */
  uint8_t p;
  m_points.clear();
  for( std::string::size_type i = 0; i < m_text.size(); i++){ 
    p = m_text[i];
    /* Calculate the vertex and texture coordinates */
    float w = m_glyph[p].bw * m_scale;
    float h = m_glyph[p].bh * m_scale;
    float xpos = x + m_glyph[p].bl * m_scale;
    float ypos = y - (h - m_glyph[p].bt * m_scale);
    float tx = m_glyph[p].tx;
    float ty = m_glyph[p].ty;
    float bw = m_glyph[p].bw;
    float bh = m_glyph[p].bh;

    /* Advance the cursor to the start of the next character */
    if(p == '\n') { //if newline
      x = 0;
      y = y - m_line_height*m_scale;
    }

    x += m_glyph[p].ax * m_scale;
    y += m_glyph[p].ay * m_scale;

    /* Skip glyphs that have no pixels */
    if (!w || !h)
      continue;

    // 
    //  ( tx, ty )             
    //  (xpos, ypos+h) |-----| (xpos + w, ypos+h)
    //                 |     |
    //  (xpos, ypos)   |-----| (xpos + w, ypos)
    //                         (bw/m_tex_w, bh/m_tex_h) 
    //
    // left triangle vertices
    // first point of left triangle
    //
    {
    m_points.push_back(xpos);
    m_points.push_back(ypos + h);
    m_points.push_back(tx);
    m_points.push_back(ty);
#ifdef __DEBUG__
    std::cout << "(" << tx << " , " << ty << ")";
#endif
    }
    //second point of left triangle
    {
    m_points.push_back(xpos);
    m_points.push_back(ypos);
    m_points.push_back(tx);
    m_points.push_back(ty + bh/m_tex_h);
#ifdef __DEBUG__
    std::cout << "(" << tx << " , " << ty + bh/m_tex_h << ")";
#endif
    }
    //third point of left triangle
    {
    m_points.push_back(xpos+w);
    m_points.push_back(ypos);
    m_points.push_back(tx + bw/m_tex_w);
    m_points.push_back(ty + bh/m_tex_h);
#ifdef __DEBUG__
    std::cout << "(" << tx + bw/m_tex_w << " , " << ty + bh/m_tex_h << ")";
#endif
    }

    //right triangle vertices
    //first point of right triangle
    {
    m_points.push_back(xpos);
    m_points.push_back(ypos + h);
    m_points.push_back(tx);
    m_points.push_back(ty);
#ifdef __DEBUG__
    std::cout << "(" << tx << " , " << ty << ")";
#endif
    }
    //second point of right triangle
    {
    m_points.push_back(xpos + w);
    m_points.push_back(ypos);
    m_points.push_back(tx + bw/m_tex_w);
    m_points.push_back(ty + bh/m_tex_h);
#ifdef __DEBUG__
    std::cout << "(" << tx + bw/m_tex_w << " , " << ty + bh/m_tex_h << ")";
#endif
    }
    //third point of right triangle
    {
    m_points.push_back(xpos + w);
    m_points.push_back(ypos + h);
    m_points.push_back(tx + bw/m_tex_w);
    m_points.push_back(ty);
#ifdef __DEBUG__
    std::cout << "(" << tx + bw/m_tex_w << " , " << ty << ")";
#endif
    }
#ifdef __DEBUG__
    std::cout << std::endl;
#endif
  }
  m_tw = (int) x;
  m_th = (int) y;
#ifdef __DEBUG__
std::cout << "width: " << x << " ; "
<< "height: " << y << "\n"; 
#endif
}

GlTextObject::GlTextObject(std::string &&s)
{
  m_sx = m_sy = m_tex_w = m_tex_h =0;
  m_shader = -1;
  m_tex_resource = -1;
  m_text = s;
  m_scale = 0.5;
  m_f_size = DEFAULT_FONT_SIZE;
  m_ww = 0;
  m_wh = 0;
  m_tw = 0;
  m_th = 0;
  m_color = nullptr;
}

GlTextObject::GlTextObject(std::string &&s, IniManager& ini)
: GlTextObject(std::move(s))
{
  m_color = ini.get_double_list("GlText", "color");
  double scale = ini.get_double("GlText", "scale");
  if(scale != 0 ) m_scale = scale;
}

void GlTextObject::update_string
(
  std::string &&s, 
  Gl::ResourceManager& manager
)
{
  m_text = s;
  init_vertex_data();

  int buffer_size = m_points.size()*sizeof(float);

  manager.replace_gl_vbo_data(
    gl_resource_id,
    (unsigned char *)&m_points[0],
    buffer_size,
    m_points.size()/4
  );
}

int GlTextObject::get_width()
{
  return m_tw;
}

int GlTextObject::get_height()
{
  return m_th;
}

void GlTextObject::set_pos(int x, int y, Gl::ResourceManager& manager)
{
  m_sx = x;
  m_sy = m_wh - y;
  if(m_shader >= 0) {
    glm::mat4 projection = glm::ortho(
      0.0f, static_cast<GLfloat>(m_ww), 
      0.0f, static_cast<GLfloat>(m_wh)
    ) * glm::translate(
      glm::mat4(1), glm::vec3(m_sx, m_sy, 0)
    );
    std::shared_ptr<Gl::Shader> shader_ptr 
      = manager.get_shader_from_shader_id(m_shader);
    shader_ptr->Bind();
    shader_ptr->SetUniformMatrix4fv("projection", projection);
    shader_ptr->UnBind();
  }
}

// Instance of virtual function.
void GlTextObject::Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager)
{
  std::shared_ptr<Gl::Shader> shader_ptr 
    = manager.get_shader_from_shader_id(m_shader);
  shader_ptr->Bind();

  GLCall(glEnable(GL_CULL_FACE));
  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));  
  GLCall(glActiveTexture(GL_TEXTURE0));
  manager.resource_bind(gl_resource_id);
  GLCall(glBindTexture(GL_TEXTURE_2D, m_tex_resource));

  manager.gl_window_update(gl_resource_id);

  GLCall(glBindTexture(GL_TEXTURE_2D, 0));
  GLCall(glDisable(GL_BLEND));
  GLCall(glDisable(GL_CULL_FACE));
  shader_ptr->UnBind();
}

void GlTextObject::set_window_size(
  int w, 
  int h, 
  Gl::ResourceManager &manager 
)
{
  int y = m_wh - m_sy;
  m_ww = w;
  m_wh = h;
  m_sy = m_wh - y;
  //if shader has been already initialized.
  if(m_shader >= 0) {
    glm::mat4 projection = glm::ortho(
      0.0f, static_cast<GLfloat>(m_ww), 
      0.0f, static_cast<GLfloat>(m_wh)
    ) * glm::translate(
      glm::mat4(1), glm::vec3(m_sx, m_sy, 0)
    );
    std::shared_ptr<Gl::Shader> shader_ptr 
      = manager.get_shader_from_shader_id(m_shader);
    shader_ptr->Bind();
    shader_ptr->SetUniformMatrix4fv("projection", projection);
    shader_ptr->UnBind();
  }
}

void GlTextObject::init_gl_buffer(
  Gl::ResourceManager &manager, 
  Gl::VertexBufferLayout& layout,
  int shader_id
)
{

  m_shader = manager.request_gl_shader_create("shader/textv.glsl", "shader/textf.glsl");
  int vertArray_id = manager.request_gl_alloc_vertexArray();

  std::shared_ptr<Gl::Shader> shader_ptr = manager.get_shader_from_shader_id(m_shader);
  shader_ptr->Bind();

  unsigned int coord = shader_ptr->GetAttribLocation("coord");
  shader_ptr->SetUniform1i("tex", 0);
  if(m_color == nullptr)
    shader_ptr->SetUniform4f("textColor", 0.0, 0.5, 0.5, 1);
  else{
    shader_ptr->SetUniform4f(
      "textColor",
      (*m_color)[0],
      (*m_color)[1],
      (*m_color)[2],
      1
    );
  }
  Gl::VertexBufferLayout text_layout;
  text_layout.Push<float>(4, coord);

  bool ret = load_freetype_info(m_f_size);
  if(ret == false) {
    std::cout << __FILE__ << "(" << __LINE__ << ")" <<
    "init_gl_buffer() fail." << std::endl;
    return;
  }
  init_vertex_data();

  int buffer_size = m_points.size();
  if( buffer_size == 0 ) {
    std::cout << __FILE__ << "(" << __LINE__ << ")" <<
    "init_gl_buffer(): empty text." << std::endl;
    return;
  }
  else {
    buffer_size = buffer_size*sizeof(float);

  }

  gl_resource_id = manager.request_gl_vbo_data(
        (unsigned char *)&m_points[0],
        buffer_size,
        buffer_size/(4*sizeof(float)),
        GL_ARRAY_BUFFER,
        text_layout,
        GL_TRIANGLES,
        m_shader,
        vertArray_id 
        );

  shader_ptr->UnBind();
}
