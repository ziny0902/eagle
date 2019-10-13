#include <iostream>
#include <GLPP/util.h>
#include <Geo/geometry_common.h>
#include "plane3d.h"

#define ADD_VEC_4(v, x, y, z, a)                  \
  v.push_back(x);                               \
  v.push_back(y);                               \
  v.push_back(z);                               \
  v.push_back(a);

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
  ADD_VEC_3(m_fixed, -1, -1, 0);
  ADD_VEC_3(m_fixed, -1, 1, 0);
  ADD_VEC_3(m_fixed, 1, 1, 0);
  ADD_VEC_3(m_fixed, 1, -1, 0);
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
#ifdef __DEBUG__
  std::cout << __FILE__ << __LINE__ << " " << num_of_vertex << "\n";
#endif

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
  ADD_VEC_4(m_tex, color.x, color.y, color.z, 0.25);
  ADD_VEC_3(m_tex, scale.x, scale.y, scale.z);
  ADD_VEC_3(m_tex, rotate.x, rotate.y, rotate.z);
  ADD_VEC_3(m_tex, translate.x, translate.y, translate.z);
  int bytes = (m_tex.size() - tex_offset) * sizeof(float);
  m_uniBuffer.AddData((unsigned char *)&m_tex[tex_offset], bytes);

  // vertex buffer data
  tex_offset = tex_offset/(4*4);
  ADD_VEC_2(m_dynamic, 0, tex_offset);
  ADD_VEC_2(m_dynamic, 1, tex_offset);
  ADD_VEC_2(m_dynamic, 3, tex_offset);
  ADD_VEC_2(m_dynamic, 3, tex_offset);
  ADD_VEC_2(m_dynamic, 1, tex_offset);
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
  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  GLCall(glDepthFunc(GL_ALWAYS))

  manager.gl_window_update(gl_resource_id, false);

  GLCall(glDisable(GL_BLEND));
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
      GL_TRIANGLES,
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
      GL_TRIANGLES,
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
      , "debug_out"
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

  glm::vec3 scale = glm::vec3(w, h, 1);
  glm::vec3 rotate = glm::vec3(phi, 0, theta);
  glm::vec3 vec3_color = glm::vec3(color.r, color.g, color.b);
  update_gl_data(manager, vec3_color, scale, rotate, c);
}

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/constants.hpp>
void Plane3d::caculate_norm_angle(
    glm::vec3& n
    , double& theta
    , double& phi
)
{
  glm::vec3 s_coord = rectangular_to_spherical(n);
  theta = (s_coord.y - glm::half_pi<float>()) *-1;
  phi = s_coord.z;

  std::cout << "theta: " << s_coord.y << std::endl;
  std::cout << "phi: " << s_coord.z << std::endl;

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
