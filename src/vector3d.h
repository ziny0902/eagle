#pragma once
#include <chrono>

#include <glm/glm.hpp>
#include <Geo/geometry_3d_object.h>
#include <GLPP/glBuffer.h>
#include <GLPP/vertex_arrays.h>
#include <GLPP/shader.h>
#include <GLPP/gl_drawing_obj.h>
#include "Highlight.h"

using namespace std::chrono;

// Inheritance from Drawable_obj. 
// List of property from Inheritance.
//   - gl_resource_id.
//   - init_gl_buffer().
//   - Update().
//   - get_gl_resouce_id().

#define NUM_OF_ELEMENT_PER_VECTOR 18
#define NUM_OF_INDICES_PER_VECTOR 6 
#define HIGHLIGHT_LINE_WIDTH 5

class Vector3d : public Gl::Drawable_obj{
public:
  Vector3d() {}
  Vector3d(point_3d s, point_3d e); 
  ~Vector3d() {}
  void add_vector(point_3d s, point_3d e);
  void add_vector(Gl::ResourceManager&, point_3d s, point_3d e);
  int bytes() { return m_data.size()*sizeof(float);}
  int size() { return m_data.size(); }
  float * getArray() { return &m_data[0]; }

  // Instance of virtual function.
  void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager);
  void init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id
  );

  int find_vector(float x, float y, float z);
  void delete_highlight_vector(
    Gl::ResourceManager &manager
    , glm::vec3 coord
  );
  bool get_vector_info(int offset, glm::vec3& s, glm::vec3& e);
  void set_color(glm::vec4& color);
  void set_highligt_color(glm::vec4& color);
  std::shared_ptr<std::vector<float>> get_vector(int offset);
  void set_highlight(Gl::ResourceManager& manager
                     , Highlight& highlight
                     , glm::vec3&& coord
                     , std::string& msg
                     );
private:
  glm::vec4 m_color;
  glm::vec4 m_highlight_color;
  float m_v_head_len;
  std::vector<float> m_data;
};
