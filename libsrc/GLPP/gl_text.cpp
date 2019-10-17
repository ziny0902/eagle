#include <algorithm>

#include <GLPP/gl_text.h>
#include <util/font_util.h>
#include <GLPP/util.h>

#define DEFAULT_FONT_SIZE 30 

// Maximum texture width
#define MAXWIDTH 1024

using namespace Gl;

void GlTextObject::init_vertex_data()
{
  float text_h = 0;
  float text_w = 0;
  float x = 0;
  float y = 0;
  /* Loop through all characters */
  uint8_t p;
  int tex_w, tex_h;
  int l_height = m_ch_font.get_lineheight();
  m_ch_font.get_texture_size(tex_w, tex_h);

  m_points.clear();
  for( std::string::size_type i = 0; i < m_text.size(); i++){ 
    p = m_text[i];
    /* Calculate the vertex and texture coordinates */
    const Gl::glyph_info_t* const glyph = m_ch_font.get_glyph_info(p); 
    float w = glyph->bw * m_scale;
    float h = glyph->bh * m_scale;
    float xpos = x + glyph->bl * m_scale;
    float ypos = y - (h - glyph->bt * m_scale);
    float tx = glyph->tx;
    float ty = glyph->ty;
    float bw = glyph->bw;
    float bh = glyph->bh;

    /* Advance the cursor to the start of the next character */
    if(p == '\n') { //if newline
      text_w = std::max(text_w, x);
      text_h += l_height*m_scale;
      x = 0;
      y = y - l_height*m_scale;
    }

    x += glyph->ax * m_scale;
    y += glyph->ay * m_scale;

    /* Skip glyphs that have no pixels */
    if (!w || !h)
      continue;

    // 
    //  ( tx, ty )             
    //  (xpos, ypos+h) |-----| (xpos + w, ypos+h)
    //                 |     |
    //  (xpos, ypos)   |-----| (xpos + w, ypos)
    //                         (bw/tex_w, bh/tex_h) 
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
    m_points.push_back(ty + bh/tex_h);
#ifdef __DEBUG__
    std::cout << "(" << tx << " , " << ty + bh/tex_h << ")";
#endif
    }
    //third point of left triangle
    {
    m_points.push_back(xpos+w);
    m_points.push_back(ypos);
    m_points.push_back(tx + bw/tex_w);
    m_points.push_back(ty + bh/tex_h);
#ifdef __DEBUG__
    std::cout << "(" << tx + bw/tex_w << " , " << ty + bh/tex_h << ")";
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
    m_points.push_back(tx + bw/tex_w);
    m_points.push_back(ty + bh/tex_h);
#ifdef __DEBUG__
    std::cout << "(" << tx + bw/tex_w << " , " << ty + bh/tex_h << ")";
#endif
    }
    //third point of right triangle
    {
    m_points.push_back(xpos + w);
    m_points.push_back(ypos + h);
    m_points.push_back(tx + bw/tex_w);
    m_points.push_back(ty);
#ifdef __DEBUG__
    std::cout << "(" << tx + bw/tex_w << " , " << ty << ")";
#endif
    }
#ifdef __DEBUG__
    std::cout << std::endl;
#endif
  }
  text_w = std::max(text_w, x);
  text_h += l_height*m_scale;
  m_tw = (int) text_w;
  m_th = (int) text_h;
#ifdef __DEBUG__
std::cout << "width: " << x << " ; "
<< "height: " << y << "\n"; 
#endif
}

void GlTextObject::init()
{
  m_tx = m_ty = 0;
  m_x = m_y = 0;
  m_shader = -1;
  m_scale = 0.5;
  m_ww = 0;
  m_wh = 0;
  m_tw = 0;
  m_th = 0;
  m_color = nullptr;
  m_margin = 8;
}

GlTextObject::GlTextObject(
  std::string &&s,
  ResourceManager& res,
  IniManager& ini,
  Gl::GlCharacterTable &_ch_font
) : GlDrawableWidget(res, ini, _ch_font)
{
  init();
  m_text = s;
  m_color = ini.get_double_list("GlText", "color");
  double scale = ini.get_double("GlText", "scale");
  if(scale != 0 ) m_scale = scale;
  init_gl_buffer();
}

void GlTextObject::update_string (std::string &&s)
{
  m_text = s;
  init_vertex_data();

  int buffer_size = m_points.size()*sizeof(float);

  m_res.replace_gl_vbo_data(
      gl_resource_id,
      (unsigned char *)&m_points[0],
      buffer_size,
      m_points.size()/4
                            );
  GlDrawableWidget::resize(
      (m_tw + 2*m_margin) > m_width ? (m_tw + 2*m_margin) : m_width
      ,
      (m_th + 2*m_margin) > m_height? (m_th + 2*m_margin) : m_height
                           );
}

void GlTextObject::update_string
(
  int x,
  int y,
  std::string &&s
)
{
  update_string(std::move(s));
  set_pos(x, y);
}

int GlTextObject::get_width()
{
  return m_tw;
}

int GlTextObject::get_height()
{
  return m_th;
}

void GlTextObject::move(int x, int y)
{
  // GlWidget::move(x, y);
  set_pos(x, y);
}

void GlTextObject::set_pos(int x, int y)
{
  int l_height = m_ch_font.get_lineheight();
  m_tx = x + m_margin;
  m_x = x;
  m_ty = m_wh - y - l_height*m_scale - m_margin;
  m_y = m_wh - y - m_height; //+ m_th + m_margin;
  if(m_shader >= 0) {
    glm::mat4 projection = glm::ortho(
      0.0f, static_cast<GLfloat>(m_ww), 
      0.0f, static_cast<GLfloat>(m_wh)
    ) * glm::translate(
      glm::mat4(1), glm::vec3(m_tx, m_ty, 0)
    );
    std::shared_ptr<Gl::Shader> shader_ptr 
      = m_res.get_shader_from_shader_id(m_shader);
    shader_ptr->Bind();
    shader_ptr->SetUniformMatrix4fv("projection", projection);
    shader_ptr->UnBind();
  }
}

// Instance of virtual function.
void GlTextObject::update()
{
  GlDrawableWidget::update();

  std::shared_ptr<Gl::Shader> shader_ptr 
    = m_res.get_shader_from_shader_id(m_shader);
  shader_ptr->Bind();

  GLCall(glEnable(GL_CULL_FACE));
  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));  

  GLuint tex_resource = m_ch_font.get_texture_name();
  GLCall(glActiveTexture(GL_TEXTURE0));
  m_res.resource_bind(gl_resource_id);
  GLCall(glBindTexture(GL_TEXTURE_2D, tex_resource));

  m_res.gl_window_update(gl_resource_id);

  GLCall(glBindTexture(GL_TEXTURE_2D, 0));
  GLCall(glDisable(GL_BLEND));
  GLCall(glDisable(GL_CULL_FACE));
  shader_ptr->UnBind();
}

void GlTextObject::set_window_size(
  int w, 
  int h
)
{
  m_ty = h - (m_wh - m_ty);
  m_y = h - (m_wh - m_y);
  m_ww = w;
  m_wh = h;
  //if shader has been already initialized.
  if(m_shader >= 0) {
    glm::mat4 projection = glm::ortho(
      0.0f, static_cast<GLfloat>(m_ww), 
      0.0f, static_cast<GLfloat>(m_wh)
    ) * glm::translate(
      glm::mat4(1), glm::vec3(m_tx, m_ty, 0)
    );
    std::shared_ptr<Gl::Shader> shader_ptr 
      = m_res.get_shader_from_shader_id(m_shader);
    shader_ptr->Bind();
    shader_ptr->SetUniformMatrix4fv("projection", projection);
    shader_ptr->UnBind();
  }
}

void GlTextObject::init_gl_buffer()
{

  m_shader = m_res.request_gl_shader_create("shader/textv.glsl", "shader/textf.glsl");
  int vertArray_id = m_res.request_gl_alloc_vertexArray();

  std::shared_ptr<Gl::Shader> shader_ptr = m_res.get_shader_from_shader_id(m_shader);
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

  init_vertex_data();
  GlDrawableWidget::resize(
      (m_tw + 2*m_margin) > m_width ? (m_tw + 2*m_margin) : m_width
      ,
      (m_th + 2*m_margin) > m_height? (m_th + 2*m_margin) : m_height
  );

  int buffer_size = m_points.size();
  if( buffer_size == 0 ) {
    std::cout << __FILE__ << "(" << __LINE__ << ")" <<
    "init_gl_buffer(): empty text." << std::endl;
    return;
  }
  else {
    buffer_size = buffer_size*sizeof(float);

  }

  gl_resource_id = m_res.request_gl_vbo_data(
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
