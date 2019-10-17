#include <iostream>
#include "mesh.h"

Mesh::Mesh(int r, int c) 
{
  m_r = r;
  m_c = c;
  vao_resource_id = -1;
  ibo_resource_id = -1;
  ibo_fill_resource_id = -1;
  cal_func = nullptr;
}
void Mesh::regfunc(std::vector<float> x_range, std::vector<float> y_range, func_uv func_ptr)
{
  cal_func = func_ptr;
  float x_tick = (x_range[1] - x_range[0])/m_c;
  float y_tick = (y_range[1] - y_range[0])/m_r;
  for( int i = 0; i <= m_r; i++){
    for(int j = 0; j <= m_c; j++) {
      float u = x_range[0] + j*x_tick;
      float v = y_range[0] + i*y_tick;
      glm::vec3 vertice = cal_func(u, v);
      m_vertices.push_back(vertice);
    }
  }
  for (int y = 0; y <= m_r; y++) {
    for (int x = 0; x < m_c; x++) {
      m_indices.push_back(y * (m_c+1) + x);
      m_indices.push_back(y * (m_c+1) + x + 1);
    }
  }

  for (int x = 0; x <= m_c; x++) {
    for (int y = 0; y < m_r; y++) {
      m_indices.push_back(y * (m_r+1) + x);
      m_indices.push_back((y+1) * (m_r+1) + x);
    }
  }

  for (int y = 0; y < m_r; y++) {
    for (int x = 0; x < m_c; x++) {
      m_indices_fill.push_back(y * (m_c+1) + x);
      m_indices_fill.push_back(y * (m_c+1) + x + 1);
      m_indices_fill.push_back((y + 1) * (m_c+1)+ x + 1);

      m_indices_fill.push_back(y * (m_c+1) + x);
      m_indices_fill.push_back((y + 1) * (m_c+1) + x + 1);
      m_indices_fill.push_back((y + 1) * (m_c+1) + x);
    }
  }

}

void Mesh::init_gl_buffer(
  Gl::ResourceManager &manager, 
  Gl::VertexBufferLayout& layout,
  int shader_id
)
{
  int vertArray_id = manager.request_gl_alloc_vertexArray();

  vao_resource_id = manager.request_gl_vbo_data(
        (unsigned char *)&(m_vertices[0].x),
        m_vertices.size()*sizeof(float)*3,
        m_vertices.size(),
        GL_ARRAY_BUFFER,
        layout,
        GL_LINE_STRIP,
        shader_id,
        vertArray_id 
        );

  ibo_resource_id = manager.request_gl_vbo_data(
        (unsigned char *)&(m_indices[0]),
        m_indices.size()*sizeof(unsigned short),
        m_indices.size(),
        GL_ELEMENT_ARRAY_BUFFER,
        layout,
        GL_LINES,
        shader_id,
        vertArray_id 
        );

  ibo_fill_resource_id = manager.request_gl_vbo_data(
        (unsigned char *)&(m_indices_fill[0]),
        m_indices_fill.size()*sizeof(unsigned short),
        m_indices_fill.size(),
        GL_ELEMENT_ARRAY_BUFFER,
        layout,
        GL_TRIANGLES,
        shader_id,
        vertArray_id 
        );
  m_vertices.clear();
  m_indices.clear();
  m_indices_fill.clear();
}

void Mesh::Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager)
{
  std::shared_ptr<Gl::Shader> shader = manager.get_shader_from_element_id(vao_resource_id);
  shader->Bind();

  glDepthFunc(GL_LESS);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);

  shader->SetUniform4f("u_Color", 0.5, 0.5, 0.5, 1);
  manager.gl_window_update(ibo_fill_resource_id);

  glPolygonOffset(0, 0);
  glDisable(GL_POLYGON_OFFSET_FILL);

  GLCall(glStencilMask(0x00));
  shader->SetUniform4f("u_Color", 4, 4, 0.8, 1);
  manager.gl_window_update(ibo_resource_id, true);

  shader->UnBind();
}

void Mesh::set_highlight(
    Gl::ResourceManager& manager,
    Highlight& highlight
                         )
{
  highlight.set_highlight(manager, NULL, 0, ibo_fill_resource_id, ibo_resource_id);
}
