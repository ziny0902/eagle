#pragma once
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <GLPP/glBuffer.h>
#include <GLPP/vertex_arrays.h>
#include <GLPP/shader.h>
#include <GLPP/gl_drawing_obj.h>

typedef float (*func_xy)(float, float);

class Mesh : Gl::Drawable_obj{
public:
  Mesh(int r, int c);
  void regfunc(std::vector<float> x_range, std::vector<float> y_range, func_xy);

  // Instance of virtual function.
  void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager);
  void init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id
  );
  

private:
  int m_r, m_c; //number of row, col
  std::vector<glm::vec3> m_vertices;
  std::vector<unsigned short> m_indices;
  std::vector<unsigned short> m_indices_fill;
  int vao_resource_id, ibo_resource_id, ibo_fill_resource_id;
};
