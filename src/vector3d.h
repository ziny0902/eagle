#pragma once
#include <chrono>
#include <Geo/geometry_3d_object.h>
#include <GLPP/glBuffer.h>
#include <GLPP/vertex_arrays.h>
#include <GLPP/shader.h>
#include <GLPP/gl_drawing_obj.h>

using namespace std::chrono;

// Inheritance from Drawable_obj. 
// List of property from Inheritance.
//   - gl_resource_id.
//   - init_gl_buffer().
//   - Update().
//   - get_gl_resouce_id().
class Vector3d : Gl::Drawable_obj{
public:
  Vector3d() {m_total_bytes = 0;}
  Vector3d(point_3d s, point_3d e); 
  ~Vector3d() {}
  void add_vector(point_3d s, point_3d e);
  int bytes() { return m_total_bytes;}
  int size() { return m_data.size(); }
  float * getArray() { return &m_data[0]; }

  // Instance of virtual function.
  void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager);
  void init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id
  );

private:
  int m_total_bytes; 
  float m_v_head_len;
  std::vector<float> m_data;
};
