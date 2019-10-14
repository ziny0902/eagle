#include "Highlight.h"

Highlight::Highlight()
    : m_color(1)
{
  m_selected_oid = -1;
  m_sub_id = -1;
}

Highlight::~Highlight()
{}

#include <limits>
void Highlight::Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager)
{
  if (m_selected_oid == std::numeric_limits<unsigned int>::max())
    return;

  std::shared_ptr<Gl::Shader> shader_ptr =
      manager.get_shader_from_element_id(gl_resource_id);

  shader_ptr->Bind();
  shader_ptr->SetUniform4f(
      "u_Color",
      m_color.x,
      m_color.y,
      m_color.z,
      m_color.a
                           );
  GLCall(glLineWidth(5));

  manager.gl_window_update(gl_resource_id);

  GLCall(glLineWidth(1));
  shader_ptr->UnBind();
}

void Highlight::init_gl_buffer(
    Gl::ResourceManager &manager,
    Gl::VertexBufferLayout& layout,
    int shader_id
                    )
{
  int vertArray_id = manager.request_gl_alloc_vertexArray();
  gl_resource_id = manager.request_gl_vbo_data(
      NULL,
      256,
      0,
      GL_ARRAY_BUFFER,
      layout,
      GL_LINES,
      shader_id,
      vertArray_id
                                               );
}
void Highlight::set_highlight(
    Gl::ResourceManager& manager
    , unsigned char *vertices
    , int bytes
    , unsigned int sel_id
    , unsigned int sub_id
                   )
{
  int num_of_vertices = bytes/(sizeof(float)*3);
  m_selected_oid = sel_id;
  m_sub_id = sub_id;

  manager.replace_gl_vbo_data(
      gl_resource_id
      , vertices
      , bytes
      , num_of_vertices
                      );
}

void Highlight::clear()
{
  m_selected_oid = -1;
  m_sub_id = -1;
}
