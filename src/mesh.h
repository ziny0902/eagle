#pragma once
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <GLPP/glBuffer.h>
#include <GLPP/vertex_arrays.h>
#include <GLPP/shader.h>
#include <GLPP/gl_drawing_obj.h>
#include "Highlight.h"

typedef float (*func_xy)(float, float);

// Parametric surfaces
typedef glm::dvec3 (*func_uv)(double, double);

class Mesh : public Gl::Drawable_obj{
public:
  Mesh(int r, int c);
  void regfunc(std::vector<float> x_range, std::vector<float> y_range, func_uv);

  // Instance of virtual function.
  void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager);
  void init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id
  );
  virtual bool is_match(unsigned int id)
  {
    if(ibo_resource_id == id || ibo_fill_resource_id == id)
      return true;
    return false;
  }
  void set_highlight(Gl::ResourceManager& manager
                     , Highlight& highlight
                     , glm::vec3&& coord
                     , std::string& msg
                    );
  void selected(
      Gl::ResourceManager& manager
      , Highlight& highlight
      , glm::vec3 xyz
      , float& u
      , float& v
                      );
  void find_uv_parameter(glm::vec3 xyz, float& u, float& v);

  func_uv cal_func;

private:
  int m_r, m_c; //number of row, col
  float m_u_tick, m_v_tick;
  float m_u_s, m_v_s;
  std::vector<glm::vec3> m_vertices;
  std::vector<unsigned short> m_indices;
  std::vector<unsigned short> m_indices_fill;
  int vao_resource_id, ibo_resource_id, ibo_fill_resource_id;

};
