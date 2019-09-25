#pragma once
#include <vector>
#include <chrono>
#include <string>
#include <Geo/geometry_3d_object.h>
#include "gl_drawing_obj.h"

typedef glm::vec3 (*action_func)(float t);

class Object3D : Gl::Drawable_obj{
public:
  //Delete default construct.
  Object3D();
  Object3D(std::string &&wkt);
  Object3D(std::string &wkt);

  void action_start() {m_t0 = steady_clock::now(); m_is_moving = true;}
  void action_stop() {m_is_moving = false;}

  void regfunc(action_func);
  void move(steady_clock::time_point &t_c, Gl::ResourceManager& manager, std::string gl_attr); 

  // Instance of virtual function.
  void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager);
  void init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id
  );
private:
  void initialize_variable();
  void init_obj(std::string &wkt);
  std::vector<float> m_obj;
  unsigned short *m_indices, *m_fill_indices;
  action_func m_func_ptr;
  bool m_is_movable;
  bool m_is_moving;
  int num_of_indices;
  int num_of_fill_indices;
  int vao_resource_id, ibo_resource_id, ibo_fill_id;
  steady_clock::time_point m_t0;

};
