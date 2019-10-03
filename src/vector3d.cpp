#include <util/ini.h>
#include "vector3d.h"

Vector3d::Vector3d(point_3d s, point_3d e)
{
  m_total_bytes = 0;

  // initialize vector config from file(graph3d.ini) 
  IniManager ini("config/graph3d.ini");
  m_v_head_len = ini.get_double("GlApp", "vector_head_length");

  gl_resource_id = -1;

  add_vector(s, e);
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
  m_total_bytes += vector3d.size();
std::cout << "total bytes: "<< m_total_bytes << std::endl;
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
  for(int i = 0; i < len; i += 18) {
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

void Vector3d::delete_vector(
  Gl::ResourceManager &manager, 
  int offset
)
{
  if( m_data.size() < offset+18+1) return;
  m_data.erase(
    m_data.begin()+offset, 
    m_data.begin()+offset+18
  );

  //GL buffer update.
  manager.replace_gl_vbo_data(
    gl_resource_id,
    (unsigned char *)getArray(),
    bytes(),
    m_data.size()/3
  );
  
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
  manager.gl_window_update(gl_resource_id);
}


