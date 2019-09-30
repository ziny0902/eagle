#include <GLPP/glDrawableWidget.h>

using namespace Gl;

GlDrawableWidget::GlDrawableWidget(
  ResourceManager& res,
  IniManager& ini
)
  : m_res(res)
{
  std::shared_ptr<std::string> vs
    = ini.get_string("Widget", "vertex_shader");
  std::shared_ptr<std::string> fs
    = ini.get_string("Widget", "fragment_shader");

  m_shader_id = m_res.request_gl_shader_create(
    vs->c_str(),
    fs->c_str()
  );

  is_hidden_window = false;

}
void GlDrawableWidget::resize(const int w, const int h)
{
  GlWidget::resize(w, h); 
  // if it didn't already create gl object
  // then a call to update will create gl object
  if(m_is_realize == false) return;

  // if widget has created gl object 
  // the widget has to update gl object.
  unsigned char *data = (unsigned char *)get_vertex_bg_data();
  int bytes = get_vertex_data_bg_bytes();
  m_res.replace_gl_vbo_data(
    m_element_bg,
    data,
    bytes,
    6
  );
  data = (unsigned char *)get_vertex_fg_data();
  bytes = get_vertex_data_fg_bytes();
  m_res.replace_gl_vbo_data(
    m_element_fg,
    data,
    bytes,
    4
  );
}

void GlDrawableWidget::update()
{
  if(is_hidden_window) return;

  std::shared_ptr<Shader> shader 
    = m_res.get_shader_from_shader_id(m_shader_id);
  shader->Bind();

  if ( m_is_realize == false){
    unsigned char *data = (unsigned char *)get_vertex_bg_data(); 
    int bytes = get_vertex_data_bg_bytes();
    int coord = shader->GetAttribLocation("coord"); 
    Gl::VertexBufferLayout widget_layout;
    widget_layout.Push<float>(3, coord);
    int va_id = m_res.request_gl_alloc_vertexArray();

    m_element_bg = m_res.request_gl_vbo_data(
      data,
      bytes,
      bytes/(3*sizeof(float)),
      GL_ARRAY_BUFFER,
      widget_layout,
      GL_TRIANGLES,
      m_shader_id,
      va_id
    );

    data = (unsigned char *)get_vertex_fg_data(); 
    bytes = get_vertex_data_fg_bytes();
    m_element_fg = m_res.request_gl_vbo_data(
      data,
      bytes,
      bytes/sizeof(unsigned short),
      GL_ELEMENT_ARRAY_BUFFER,
      widget_layout,
      GL_LINE_LOOP,
      m_shader_id,
      va_id
    );

    m_is_realize = true;
    shader->Bind();
  }
  glm::mat4 ortho = glm::ortho(
      0.0f, static_cast<GLfloat>(m_ww), 
      0.0f, static_cast<GLfloat>(m_wh)
  );

  glm::mat4 projection 
    = ortho * glm::translate(
      glm::mat4(1.0f), 
      glm::vec3(m_x, m_y, 0)
      );
  shader->SetUniformMatrix4fv("projection", projection);

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);

  // set color 
  shader->SetUniform4f("U_color", m_bg.r, m_bg.g, m_bg.b, m_bg.a);
  m_res.gl_window_update(m_element_bg);

  shader->SetUniform4f("U_color", m_fg.r, m_fg.g, m_fg.b, m_fg.a);
  glPolygonOffset(0, 0);
  glDisable(GL_POLYGON_OFFSET_FILL);

  m_res.gl_window_update(m_element_fg);

  shader->UnBind();
}

