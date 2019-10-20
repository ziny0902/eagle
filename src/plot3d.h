#pragma once
#include <vector>
#include <chrono>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <GLPP/glBuffer.h>
#include <GLPP/vertex_arrays.h>
#include <GLPP/shader.h>
#include <GLPP/gl_drawing_obj.h>
#include "Highlight.h"

using namespace std::chrono;

typedef glm::dvec3 (*plot_func)(double);

class Plot3D : public Gl::Drawable_obj{
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

  std::shared_ptr<float[]> cal_func(float t);
  std::shared_ptr<float[]> tangent_vector(float t);
  std::shared_ptr<float[]> normal_vector(float t);

  // Instance of virtual function.
  void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager);
  void init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id
  );

  void set_highlight(
      Gl::ResourceManager& manager
      , Highlight& highlight
      , glm::vec3&& coord
      , std::string& msg
                     );

  double find_parameter(glm::dvec3&& coord);

private:
  std::vector<float> m_data;
  plot_func m_func_ptr;
  steady_clock::time_point m_t0;
  bool is_realtime;

};
