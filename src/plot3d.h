#pragma once
#include <vector>
#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include <GLPP/glBuffer.h>
#include <GLPP/vertex_arrays.h>
#include <GLPP/shader.h>
#include "gl_drawing_obj.h"

using namespace std::chrono;

typedef glm::vec3 (*plot_func)(float );

class Plot3D : Gl::Drawable_obj{
public:
  Plot3D();
  ~Plot3D(){}
  void regfunc(plot_func func_ptr) {m_func_ptr = func_ptr;}
  void activate_sampler(steady_clock::time_point &t_c);
  void sampling(float s, float e, float step);
  // void gl_buffer_bind(Gl::Shader &shader);
  void activate_realtime();
  void deactivate_realtime();
  void activate_sampler(steady_clock::time_point &t_c, Gl::ResourceManager& manager);

  // Instance of virtual function.
  void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager);
  void init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id
  );

private:
  std::vector<float> m_data;
  plot_func m_func_ptr;
  steady_clock::time_point m_t0;
  bool is_realtime;
};
