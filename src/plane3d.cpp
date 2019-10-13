#include <iostream>
#include <GLPP/util.h>
#include <Geo/geometry_common.h>
#include "plane3d.h"

static float box_vertex[] = {
    -1, -1, 0
  , -1,  1, 0
  ,  1,  1, 0
  ,  1, -1, 0
};

#define ADD_VEC_3(v, x, y, z) \
  v.push_back(x); \
  v.push_back(y); \
  v.push_back(z); \
  v.push_back(1);

#define ADD_VEC_2(v, x, y) \
  v.push_back(x); \
  v.push_back(y)

void Plane3d::init_fixed_data()
{
  ADD_VEC_3(m_fixed, 0, 0, 0);
  ADD_VEC_3(m_fixed, 0, 1, 0);
  ADD_VEC_3(m_fixed, 1, 1, 0);
  ADD_VEC_3(m_fixed, 1, 0, 0);

}

void Plane3d::update_gl_data(
    Gl::ResourceManager &manager
    , glm::vec3& color
    , glm::vec3& scale
    , glm::vec3& rotate
    , glm::vec3& translate
                      )
{
  unsigned int offset = m_dynamic.size() * sizeof(unsigned int);
  unsigned int idx = m_dynamic.size();
  add_dynamic_data(color, scale, rotate, translate);

  unsigned int num_of_vertex = (m_dynamic.size() - idx)/2;
  std::cout << __FILE__ << __LINE__ << " " << num_of_vertex << "\n";
  for(int i=idx; i < m_dynamic.size(); i++)
  {
    std::cout << m_dynamic[i] << "\n";
  }
  manager.add_gl_vbo_data(
      gl_resource_id,
      (unsigned char*)&m_dynamic[idx],
      num_of_vertex * 2 * sizeof(float),
      num_of_vertex
                          );
}

#include <glm/gtc/constants.hpp>
void Plane3d::add_dynamic_data(
    glm::vec3& color
    , glm::vec3& scale
    , glm::vec3& rotate
    , glm::vec3& translate
)
{

  unsigned int tex_offset = m_tex.size();
  // save texture data
  // rotate = (rotate/(2*glm::pi<float>()));
  ADD_VEC_3(m_tex, color.x, color.y, color.z);
  ADD_VEC_3(m_tex, scale.x, scale.y, scale.z);
  ADD_VEC_3(m_tex, rotate.x, rotate.y, rotate.z);
  ADD_VEC_3(m_tex, translate.x, translate.y, translate.z);
  int bytes = (m_tex.size() - tex_offset) * sizeof(float);
  m_uniBuffer.AddData((unsigned char *)&m_tex[tex_offset], bytes);

  // vertex buffer data
  // tex_offset = 1;
  // ADD_VEC_2(m_dynamic, tex_offset, 0);
  // ADD_VEC_2(m_dynamic, tex_offset, 1);
  // ADD_VEC_2(m_dynamic, tex_offset, 3);
  // ADD_VEC_2(m_dynamic, tex_offset, 2);
  ADD_VEC_2(m_dynamic, 0, tex_offset);
  ADD_VEC_2(m_dynamic, 1, tex_offset);
  ADD_VEC_2(m_dynamic, 3, tex_offset);
  ADD_VEC_2(m_dynamic, 2, tex_offset);
}
Plane3d::Plane3d() :
    m_uniBuffer(NULL, 0, GL_UNIFORM_BUFFER)
{
  m_uniBuffer_id = -1;
  init_fixed_data();
}

Plane3d::~Plane3d()
{}

void Plane3d::Update(
  steady_clock::time_point &t_c
  , Gl::ResourceManager& manager
)
{
  std::shared_ptr<Gl::Shader> shader_ptr =
      manager.get_shader_from_element_id(gl_resource_id);
  shader_ptr->Bind();

  manager.gl_window_update(gl_resource_id, true);

  shader_ptr->UnBind();
}

void Plane3d::init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id
                    )
{
  int vertArray_id = manager.request_gl_alloc_vertexArray();
  if(m_dynamic.size() == 0){
    gl_resource_id = manager.request_gl_vbo_data(
      NULL,
      1024,
      0,
      GL_ARRAY_BUFFER,
      layout,
      GL_TRIANGLE_STRIP,
      shader_id,
      vertArray_id
    );
  }
  else{
    gl_resource_id = manager.request_gl_vbo_data(
      (unsigned char *)&m_dynamic[0],
      m_dynamic.size() * sizeof(float),
      m_dynamic.size()/2,
      GL_ARRAY_BUFFER,
      layout,
      GL_TRIANGLE_STRIP,
      shader_id,
      vertArray_id
    );
  }
}

void Plane3d::init_gl_buffer(
    Gl::ResourceManager &manager
                    )
{
  m_shader 
    = manager.request_gl_shader_create(
      "shader/plane3dv.glsl", "shader/plane3df.glsl"
      , "fixed_idx"
      //      , "gl_Position"
    );

  std::shared_ptr<Gl::Shader> shader_ptr
    = manager.get_shader_from_shader_id(m_shader);
  shader_ptr->Bind();

  m_uniBuffer_id = manager.alloc_unibuffer_id();
  shader_ptr->UniformBlockBinding("dynamic_block", m_uniBuffer_id);
  shader_ptr->SetUniform4fv("fixed_table", m_fixed.size()/3, &m_fixed[0]);

  m_uniBuffer.BindBufferRange(m_uniBuffer_id, 32*4*sizeof(glm::vec4));

  unsigned int v_idx= shader_ptr->GetAttribLocation("v_idx");
  unsigned int d_idx= shader_ptr->GetAttribLocation("d_idx");

  Gl::VertexBufferLayout layout;
  layout.Push<float> (1, v_idx);
  layout.Push<float> (1, d_idx);
  init_gl_buffer(manager, layout, m_shader);
}

void Plane3d::add_plane(
    Gl::ResourceManager &manager
    , glm::vec3& s
    , glm::vec3& e
    , glm::vec3& c
    , double w
    , double h
    , glm::vec4& color
)
{
  double theta, phi;

  glm::vec3 norm = glm::cross(s, e);
  caculate_norm_angle(norm, theta, phi);

  std::cout << "w: " << w << " h: " << h <<"\n";

  glm::vec3 scale = glm::vec3(w, h, 1);
  glm::vec3 rotate = glm::vec3(phi, 0, theta);
  glm::vec3 vec3_color = glm::vec3(color.r, color.g, color.b);
  update_gl_data(manager, vec3_color, scale, rotate, c);
}

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
void Plane3d::caculate_norm_angle(
    glm::vec3& n
    , double& theta
    , double& phi
)
{
  glm::vec3 s_coord = rectangular_to_spherical(n);
  std::cout << "theta: " << s_coord.y << std::endl;
  std::cout << "phi: " << s_coord.z << std::endl;
  theta = s_coord.y;
  phi = s_coord.z;

  glm::mat4 rotate_x = 
      glm::rotate(glm::mat4(1), s_coord.z, glm::vec3(1, 0, 0));
  glm::mat4 rotate_z = 
      glm::rotate(glm::mat4(1), s_coord.y, glm::vec3(0, 0, 1));
  for(int i =0; i < 4; i++)
  {
    glm::vec3 *v = (glm::vec3 *)(&box_vertex[3*i]);
    glm::vec4 conv = rotate_z * rotate_x * glm::vec4((*v), 1);
    std::cout << glm::to_string(conv) << std::endl;
  }
}

void Plane3d::UniformBlockBinding(
    Gl::ResourceManager &manager
    , const std::string& name
    , unsigned int id
)
{
  std::shared_ptr<Gl::Shader> shader_ptr
      = manager.get_shader_from_element_id(gl_resource_id);
  shader_ptr->Bind();
  shader_ptr->UniformBlockBinding(name, id);
  shader_ptr->UnBind();
}
