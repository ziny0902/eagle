#include <util/ini.h>
#include "vector3d.h"

Vector3d::Vector3d(point_3d s, point_3d e)
: m_color(1)
, m_highlight_color(1, 1, 0, 1)
{
  m_highlight_vector = -1;

  // initialize vector config from file(graph3d.ini) 
  IniManager ini("config/graph3d.ini");
  m_v_head_len = ini.get_double("GlApp", "vector_head_length");

  gl_resource_id = -1;

  add_vector(s, e);
}

void Vector3d::set_color(glm::vec4& color)
{
  m_color = color;
}

void Vector3d::set_highligt_color(glm::vec4& color)
{
  m_highlight_color = color;
}

void Vector3d::add_vector(point_3d s, point_3d e)
{
  geo::GeoModel3D vector3d;
  vector3d.append(s);
  vector3d.append(e);
  vector3d.add_arrow_head(geo::GEO_S_E, m_v_head_len);
  int data_cnt = vector3d.size()/sizeof(float);
  float *ptr = vector3d.getArray();
  for(int i = 0; i < data_cnt; i++){
    m_data.push_back(ptr[i]);
  }
}

bool Vector3d::get_vector_info(int offset, glm::vec3& s, glm::vec3& e)
{
  if( offset+NUM_OF_ELEMENT_PER_VECTOR > m_data.size() 
      || offset < 0
      || (offset%NUM_OF_ELEMENT_PER_VECTOR) != 0
  ) 
  {
    std::cout << "Error: Vector3d::get_vector_info\n"
    << "Invalid offset\n";
    return false;
  }
  s.x = m_data[offset];
  s.y = m_data[offset+1];
  s.z = m_data[offset+2];

  e.x = m_data[offset+3];
  e.y = m_data[offset+4];
  e.z = m_data[offset+5];

  return true;
}

void Vector3d::highlight(int offset)
{
  m_highlight_vector = offset;
}

void Vector3d::add_vector(
Gl::ResourceManager &manager,
point_3d s, point_3d e
)
{
  add_vector(s, e);

  if( gl_resource_id > 0) {
    manager.replace_gl_vbo_data(
            gl_resource_id,
            (unsigned char *)getArray(),
            bytes(),
            m_data.size()/3
          );
  }
}

#include <limits>
int Vector3d::find_vector(float x, float y, float z)
{
  size_t len = m_data.size();
  int c_distance = std::numeric_limits<int>::max();
  int offset = 0;
  for(int i = 0; i < len; i += NUM_OF_ELEMENT_PER_VECTOR) {
    geo::GeoModel3D vector3d;
    vector3d.append({m_data[i], m_data[i+1], m_data[i+2]});
    vector3d.append({m_data[i+3], m_data[i+4], m_data[i+5]});
    int distance = vector3d.distance({x, y, z});
    if( distance < c_distance ) {
      offset = i;
      c_distance = distance;
    }
  }
  return offset;
}

void Vector3d::delete_highlight_vector(
  Gl::ResourceManager &manager
)
{
  if( m_data.size() < m_highlight_vector+NUM_OF_ELEMENT_PER_VECTOR) return;
  m_data.erase(
    m_data.begin()+m_highlight_vector, 
    m_data.begin()+m_highlight_vector+NUM_OF_ELEMENT_PER_VECTOR
  );

  //GL buffer update.
  manager.replace_gl_vbo_data(
    gl_resource_id,
    (unsigned char *)getArray(),
    bytes(),
    m_data.size()/3
  );
  
  m_highlight_vector = -1;
}

void Vector3d::init_gl_buffer(
  Gl::ResourceManager &manager, 
  Gl::VertexBufferLayout& layout,
  int shader_id
)
{
  int vertArray_id = manager.request_gl_alloc_vertexArray();

  gl_resource_id = manager.request_gl_vbo_data(
        (unsigned char *)getArray(),
        bytes(),
        m_data.size()/3,
        GL_ARRAY_BUFFER,
        layout,
        GL_LINES,
        shader_id,
        vertArray_id 
  );
}

void Vector3d::Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager)
{
  if (m_highlight_vector == -1)
  {
    manager.gl_window_update(gl_resource_id);
    return;
  }
  std::shared_ptr<Gl::Shader> shader_ptr =
    manager.get_shader_from_element_id(gl_resource_id);

  int offset_idx = m_highlight_vector/3;
  int total_num_of_indices = bytes()/(3*sizeof(float));
  int num_of_indices = NUM_OF_INDICES_PER_VECTOR;
  if(offset_idx == 0) {
    shader_ptr->SetUniform4f(
      "u_Color", 
      m_highlight_color.x,
      m_highlight_color.y,
      m_highlight_color.z,
      m_highlight_color.a
    );
    GLCall(glLineWidth(HIGHLIGHT_LINE_WIDTH));
    num_of_indices = NUM_OF_INDICES_PER_VECTOR;
    manager.gl_window_update(
      gl_resource_id,
      offset_idx,
      num_of_indices
    );
  }
  else {
    manager.gl_window_update(gl_resource_id, 0, offset_idx);

    shader_ptr->SetUniform4f(
      "u_Color", 
      m_highlight_color.x,
      m_highlight_color.y,
      m_highlight_color.z,
      m_highlight_color.a
    );
    GLCall(glLineWidth(HIGHLIGHT_LINE_WIDTH));
    manager.gl_window_update(
      gl_resource_id,
      offset_idx,
      num_of_indices
    );
  }

  shader_ptr->SetUniform4f(
    "u_Color", 
    m_highlight_color.x,
    m_highlight_color.y,
    m_highlight_color.z,
    m_highlight_color.a
  );
  GLCall(glLineWidth(1));
  offset_idx += num_of_indices;
  num_of_indices = total_num_of_indices - offset_idx;
#ifdef __DEBUG__
std::cout << "offset_idx = " << offset_idx << std::endl;
std::cout << "num_of_indices = " << num_of_indices << std::endl;
#endif

  manager.gl_window_update(
    gl_resource_id,
    offset_idx,
    num_of_indices
  );

}

