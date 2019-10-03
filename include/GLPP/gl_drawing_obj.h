#pragma once
#include <chrono>
#include <GLPP/gl_resource_manager.h>

using namespace std::chrono;

namespace Gl{

// Define Interface Class.
class Drawable_obj{
public:
  Drawable_obj(){ gl_resource_id = -1; }
  virtual ~Drawable_obj(){}
  virtual void init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id
  ) = 0;
  virtual void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager) = 0;
  inline int get_gl_resouce_id() { return gl_resource_id; }
  virtual bool is_match(unsigned int id) {
    if(gl_resource_id == id) return true;
    else return false;
  }

protected:
  int gl_resource_id;
};

}
