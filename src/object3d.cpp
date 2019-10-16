#include "object3d.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Object3D::initialize_variable()
{
  m_is_movable = false;
  m_is_moving = false;
  m_func_ptr = nullptr;
  m_indices = nullptr;
  num_of_indices = 0;
  vao_resource_id = -1;
  ibo_resource_id = -1;
  m_t0 = steady_clock::now();
}

#define NUM_OF_BOX_INDICES 48
static unsigned short box3d_indices[NUM_OF_BOX_INDICES] = {
0, 1, 1, 2, 2, 3, 3, 0,
4, 5, 5, 6, 6, 7, 7, 4,
0, 4, 4, 7, 7, 3, 3, 0,
3, 2, 2, 6, 6, 7, 7, 3,
1, 5, 5, 6, 6, 2, 2, 1,
0, 1, 1, 5, 5, 4, 4, 0
};

#define NUM_OF_BOX_FILL_INDICES 36
static unsigned short box3d_fill_indices[] = {
  0, 1, 5, 5, 4, 0
  , 6, 5, 1, 1, 2, 6
  , 4, 5, 7, 7, 5, 6
  , 4, 0, 3, 3, 7, 4
  , 1, 0, 3, 3, 2, 1
  , 3, 2, 6, 6, 7, 3
};
// static unsigned short box3d_fill_indices[] = {
// 4, 5, 6, 4, 7, 6,
// 7, 4, 0, 7, 3, 0,
// 1, 0, 4, 4, 5, 1,
// 2, 1, 5, 2, 6, 5,
// 7, 3, 2, 7, 5, 2,
// 0, 1, 2, 0, 2, 3
// };

void Object3D::init_obj(std::string &wkt)
{
  geo::GeoModel3D model(wkt);
  model.getVector(m_obj);

#ifdef __DEBUG__
std::cout << __FILE__ << "("\
<< __LINE__ << ") " << m_obj.size() << std::endl;
for(int i =0; i < m_obj.size(); i++)
{
std::cout << m_obj[i] << " " ;
}
std::cout << std::endl;
#endif

  geo::GeoType type = model.getType();
  if(type = geo::GEO_BOX){
    m_indices = box3d_indices;
    num_of_indices = NUM_OF_BOX_INDICES;

    m_fill_indices = box3d_fill_indices;
    num_of_fill_indices = NUM_OF_BOX_FILL_INDICES;
#ifdef __DEBUG__
std::cout << "number of indices : " << num_of_indices << std::endl;
#endif
  }
}

Object3D::Object3D()
{
  initialize_variable();
}

Object3D::Object3D(std::string &&wkt)
{
  initialize_variable();
  init_obj(wkt);
}

Object3D::Object3D(std::string &wkt)
{
  initialize_variable();
  init_obj(wkt);
}

void Object3D::regfunc(action_func func_ptr)
{
  m_func_ptr = func_ptr;
  m_is_movable = true;
}

void Object3D::move(
  steady_clock::time_point &t_c, 
  Gl::ResourceManager& manager, 
  std::string gl_attr
)
{
  if(m_is_moving && m_is_movable)
  { }
  else{ return; }

  milliseconds t_d = duration_cast<milliseconds> (t_c - m_t0);
  double t = ((double)(t_d.count())/1000);
  glm::vec3 v = m_func_ptr(t);

  std::shared_ptr<Gl::Shader> shader 
    = manager.get_shader_from_element_id(vao_resource_id);
  glm::mat4 translate = glm::translate(glm::mat4(1.0f), v);
  shader->SetUniformMatrix4fv(gl_attr, translate);
}

void Object3D::init_gl_buffer(
  Gl::ResourceManager &manager, 
  Gl::VertexBufferLayout& layout,
  int shader_id
)
{
  int vertArray_id = manager.request_gl_alloc_vertexArray();

  vao_resource_id = manager.request_gl_vbo_data(
        (unsigned char *)&(m_obj[0]),
        m_obj.size()*sizeof(float),
        m_obj.size()/3,
        GL_ARRAY_BUFFER,
        layout,
        GL_LINES,
        shader_id,
        vertArray_id 
        );

  if(m_indices != nullptr) {
    ibo_resource_id = manager.request_gl_vbo_data(
          (unsigned char *)(m_indices),
          num_of_indices*sizeof(unsigned short),
          num_of_indices,
          GL_ELEMENT_ARRAY_BUFFER,
          layout,
          GL_LINES,
          shader_id,
          vertArray_id 
          );

    ibo_fill_id = manager.request_gl_vbo_data(
          (unsigned char *)(m_fill_indices),
          num_of_fill_indices*sizeof(unsigned short),
          num_of_fill_indices,
          GL_ELEMENT_ARRAY_BUFFER,
          layout,
          GL_TRIANGLES,
          shader_id,
          vertArray_id 
          );
  }
}

// Instance of virtual function.
void Object3D::Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager)
{
  std::shared_ptr<Gl::Shader> shader = manager.get_shader_from_element_id(vao_resource_id);

  glDepthFunc(GL_LESS);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);
  shader->SetUniform4f("u_Color", 0.5, 0, 0.5, 1);
  manager.gl_window_update(ibo_fill_id);


  glPolygonOffset(0, 0);
  glDisable(GL_POLYGON_OFFSET_FILL);
  shader->SetUniform4f("u_Color", 1, 1, 1, 1);
  manager.gl_window_update(ibo_resource_id);
}

