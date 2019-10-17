#pragma once
#include <vector>
#include <memory>
#include <epoxy/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <GLPP/glBuffer.h>
#include <GLPP/vertex_arrays.h>
#include <GLPP/shader.h>

namespace Gl{
struct ResourceElement{
  unsigned int num_of_vertex; 
  unsigned int bytes_per_vertex;
  unsigned int vertexArray_idx;
  GLenum mode;
  GLenum buffer_type;
  std::shared_ptr<Gl::GlBuffer> vbo;
  std::shared_ptr<Gl::Shader> shader;
};

class ResourceManager {
public:
  ResourceManager(int cnt=1);
  ~ResourceManager(){}

  void clear();

  int alloc_unibuffer_id()
  { m_unibuffer_id++; return m_unibuffer_id; }

  //return vertexArray index.
  int request_gl_alloc_vertexArray();

  void gl_free_vertexArray(unsigned short vertexA_idx);  

  // return shader index
  int request_gl_shader_create(
      const char *v_f
      , const char *f_f
      , const char *debug_attr = NULL
                               );

  // return element index
  int request_gl_vbo_data(unsigned char* data, 
                      unsigned int data_bytes, 
                      unsigned int num_of_vertex, 
                      GLenum buffer_type,
                      const Gl::VertexBufferLayout& layout,
                      GLenum glprimitive_mode,
                      unsigned short shader_idx,
                      unsigned short vertexA_idx,
                      bool queue_enabled = false
                      );

  void replace_gl_vbo_data(
                      unsigned short element_id,
                      unsigned char* data, 
                      unsigned int data_bytes,
                      unsigned int num_of_vertex
                      );

  bool add_gl_vbo_data(
                      unsigned short element_id,
                      unsigned char* data, 
                      unsigned int data_bytes,
                      unsigned int num_of_vertex
                      );


  std::shared_ptr<Gl::Shader> get_shader_from_element_id(unsigned short element_id);
  std::shared_ptr<Gl::Shader> get_shader_from_shader_id(unsigned short shader_id);
  void gl_window_update(
      unsigned short element_id
      , bool stancil_control = false
      , bool debug = false);
  void gl_window_update(
    unsigned short element_id
    , int offset
    , int num_of_vertex
    , bool stancil_control = false
    , bool debug = false
  ); 
  void resource_bind(unsigned short element_id);
  void resource_unbind(unsigned short element_id);

private:
  int m_unibuffer_id;
  Gl::VertexArrays m_vertArray;
  std::vector<bool> m_vertArray_alloc_table;
  std::vector<std::shared_ptr<Gl::Shader>> m_shader_list;
  std::vector<std::shared_ptr<ResourceElement>> m_element_list;
};

}
