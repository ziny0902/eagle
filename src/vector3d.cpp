#include <util/ini.h>
#include "vector3d.h"

Vector3d::Vector3d(point_3d s, point_3d e)
{
  m_total_bytes = 0;

  // initialize vector config from file(graph3d.ini) 
  IniManager ini("config/graph3d.ini");
  m_v_head_len = ini.get_double("GlApp", "vector_head_length");

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


