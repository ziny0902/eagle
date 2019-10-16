#include "Highlight.h"

Highlight::Highlight()
    : m_color(1, 1, 0, 1)
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

  glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(1.03, 1.03, 1));

  std::shared_ptr<Gl::Shader> shader_ptr =
      // is_delegated ?
      // manager.get_shader_from_element_id(m_selected_oid) :
       manager.get_shader_from_element_id(gl_resource_id);

  shader_ptr->Bind();
  // shader_ptr->SetUniformMatrix4fv("scale", scale);
  shader_ptr->SetUniform4f(
      "u_Color",
      m_color.x,
      m_color.y,
      m_color.z,
      m_color.a
                           );
  GLCall(glLineWidth(3));
  glDepthFunc(GL_LESS);


  if(is_delegated){
    // glStencilFunc(GL_NOTEQUAL, m_selected_oid, 0xFF);
    manager.gl_window_update(m_sub_id, false);
  }
  else
    manager.gl_window_update(gl_resource_id, false);

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

  is_delegated = false;

  if (vertices == NULL)
  {
    is_delegated = true;
  }
  else
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
  is_delegated = false;
}
