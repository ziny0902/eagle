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
  unsigned int offset = m_dynamic.size() * sizeof(float);
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
  glDepthFunc(GL_LESS);

  manager.gl_window_update(gl_resource_id, false, false);

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
  unsigned char *buffer = NULL;
  int bytes = 1024;
  int num_of_vertex = 0;

  if(m_dynamic.size() != 0) {
    buffer = (unsigned char *)&m_dynamic[0];
    bytes = m_dynamic.size()*sizeof(float);
    num_of_vertex = m_dynamic.size()/2;
  }
  gl_resource_id = manager.request_gl_vbo_data(
    buffer,
    bytes ,
    num_of_vertex,
    GL_ARRAY_BUFFER,
    layout,
    GL_TRIANGLES,
    shader_id,
    vertArray_id
  );

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

  std::cout << "theta: " << theta << std::endl;
  std::cout << "phi: " << phi << std::endl;

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

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
std::shared_ptr<geo::GeoModel3D>
Plane3d::get_plane_by_index(int idx)
{

  glm::vec4 points[] = {
    {-1, -1, 0, 1}
    , {-1, 1, 0, 1}
    , {1, 1, 0, 1}
    , {1, -1, 0, 1}
    , {-1, -1, 0, 1}
  };
  // plane3dv.glsl; struct plane_info
  idx = idx * 4 * 4 + 4;

  float *ptr = &m_tex[idx];
  glm::vec4 v_scale (ptr[0], ptr[1], ptr[2], ptr[3]);
  ptr += 4;
  glm::vec4 v_rotate (ptr[0], ptr[1], ptr[2], ptr[3]);
  ptr += 4;
  glm::vec3 v_translate (ptr[0], ptr[1], ptr[2]);

  glm::mat4 transform_scale
      = glm::scale(glm::mat4(1.0f), glm::vec3(v_scale.x, v_scale.y, 1.0f));
  glm::mat4 transform_rotateX
      = glm::rotate(glm::mat4(1.0f), -1*v_rotate.x, glm::vec3(1, 0, 0));
  glm::mat4 transform_rotateZ
      = glm::rotate(glm::mat4(1.0f), -1*v_rotate.z, glm::vec3(0, 0, 1));
  glm::mat4 transform_translate
      = glm::translate(glm::mat4(1.0f), v_translate);
  glm::mat4 transform
      = transform_translate
      * transform_rotateZ
      * transform_rotateX
      * transform_scale;
  std::shared_ptr<geo::GeoModel3D> plane
      = std::make_shared<geo::GeoModel3D>();
  for(int i=0; i < 5; i++)
  {
    points[i] = transform * points[i];
    std::cout << glm::to_string(points[i]) << "\n";
    plane->append({points[i].x, points[i].y, points[i].z});
  }

  return plane;
}

int Plane3d::find_plane(float x, float y, float z)
{
  double c_distance = std::numeric_limits<int>::max();
  int num_of_plane = m_tex.size()/(4*4);
  int idx = -1;
  std::shared_ptr<geo::GeoModel3D> select = nullptr;
  std::cout << "num_of_plane "  << num_of_plane << std::endl;

  for(int i = 0; i < num_of_plane; i++)
  {
    std::shared_ptr<geo::GeoModel3D> plane
        = get_plane_by_index(i);
    double distance = plane->distance(geo::GEO_POLYGON, {x, y, z});
    std::cout << "distance: " << distance << std::endl;
    if(distance < c_distance) {
      idx = i;
      c_distance = distance;
      select = plane;
    }
  }

  return idx;
}

bool Plane3d::set_selected_plane(
    Gl::ResourceManager& manager
    , Highlight& highlight
    , glm::vec3& coord
)
{
  int idx = find_plane(coord.x, coord.y, coord.z);
  if(idx < 0) return false;
  std::shared_ptr<geo::GeoModel3D> ptr = get_plane_by_index(idx);
  if (ptr == nullptr) {
    return false;
  }

  float *arr = ptr->getArray();
  int len = ptr->size()/sizeof(float);
  std::vector<float> v;
  for(int i = 0; i < len; i+=3)
  {
    v.push_back(arr[i]);
    v.push_back(arr[i+1]);
    v.push_back(arr[i+2]);
    if(i != 0 && i+3 < len){
      v.push_back(arr[i]);
      v.push_back(arr[i+1]);
      v.push_back(arr[i+2]);
    }
  }
#ifdef __DEBUG__
  for(int i=0; i < v.size(); i+=3)
  {
    std::cout << "("
              << v[i]
              << ","
              << v[i+1]
              << ","
              << v[i+2]
              << ")\n";
  }
#endif
  highlight.set_highlight(
      manager
      , (unsigned char *)&v[0]
      , v.size()*sizeof(float)
      , gl_resource_id
      , idx
                          );

  return true;
}
