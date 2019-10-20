#include "Highlight.h"

static float marker[] = {
  0, 0, 0
  , 0.4, 0.4, 0
  , -0.4, 0.4, 0
};

Highlight::Highlight()
    : m_color(1, 1, 0, 1)
{
  m_selected_oid = -1;
  m_sub_id = -1;
  m_selected_object = app_common::app_gl_object::none;
}

Highlight::~Highlight()
{}

#include <limits>
void Highlight::Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager)
{
  if (m_selected_object == app_common::app_gl_object::none)
    return;
  if (m_selected_oid == std::numeric_limits<unsigned int>::max())
    return;

  static glm::mat4 Identity = glm::mat4(1.0f);

  std::shared_ptr<Gl::Shader> shader_ptr =
       manager.get_shader_from_element_id(gl_resource_id);
  shader_ptr->Bind();

  shader_ptr->SetUniformMatrix4fv("trans", Identity);

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

  shader_ptr->SetUniform4f(
      "u_Color"
      , 1, 0, 1, 1
                           );
  if(m_selected_object != app_common::app_gl_object::vector3d
     && m_selected_object != app_common::app_gl_object::plane3d)
  {
    glDepthFunc(GL_ALWAYS);
    glPointSize(7.0);
    manager.gl_window_update(m_marker_resource);
    glPointSize(1.0);
    glDepthFunc(GL_LESS);
  }

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

  vertArray_id = manager.request_gl_alloc_vertexArray();
  m_marker_resource = manager.request_gl_vbo_data(
      NULL,
      // 3*3*sizeof(float),
      3*sizeof(float),
      0,
      GL_ARRAY_BUFFER,
      layout,
      GL_POINTS,
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
    , app_common::app_gl_object sel_obj
                   )
{
  int num_of_vertices = bytes/(sizeof(float)*3);
  m_selected_oid = sel_id;
  m_sub_id = sub_id;
  m_selected_object = sel_obj;

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
  m_selected_object = app_common::app_gl_object::none;
}

void Highlight::set_selected_coord(
    Gl::ResourceManager& manager,
    glm::vec3&& coord)
{
  set_selected_coord(manager, coord);
}

void Highlight::set_selected_coord(
    Gl::ResourceManager& manager
    ,glm::vec3& coord)
{
  m_coord = coord;
  float vertices[9];

#ifdef __DEBUG__
  std::cout << "( " << coord.x
            << ", " << coord.y
            << ", " << coord.z << std::endl;
#endif

  for(int i=0; i < 9; i += 3)
  {
    vertices[i] = marker[i] + coord.x;
    vertices[i+1] = marker[i+1] + coord.y;
    vertices[i+2] = marker[i+2] + coord.z;
  }
  manager.replace_gl_vbo_data(
      m_marker_resource
      , (unsigned char *)vertices
      // , 9*sizeof(float)
      , 3*sizeof(float)
      , 1
      // , 3
                              );
}
