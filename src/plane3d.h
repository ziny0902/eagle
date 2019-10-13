#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GLPP/gl_drawing_obj.h>

class Plane3d : public Gl::Drawable_obj{
 public:
  Plane3d();
  ~Plane3d();
  void add_plane(
      Gl::ResourceManager &manager
      , glm::vec3& s
      , glm::vec3& e
      , glm::vec3& c
      , double w
      , double h
      , glm::vec4& color
                 );
  virtual void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager);

  void init_gl_buffer(
      Gl::ResourceManager &manager, 
      Gl::VertexBufferLayout& layout,
      int shader_id
                      );
  void init_gl_buffer(
      Gl::ResourceManager &manager
                      );
  void update_gl_data(
      Gl::ResourceManager &manager
      , glm::vec3& color
      , glm::vec3& scale
      , glm::vec3& rotate
      , glm::vec3& translate
                        );

  void UniformBlockBinding(
      Gl::ResourceManager &manager
      , const std::string& name
      , unsigned int id
                                    );

 private:
  // OpenGL Resource.
  unsigned int m_tex_resource;
  unsigned int ibo_resource;
  unsigned int m_shader;
  Gl::GlBuffer m_uniBuffer;
  int m_uniBuffer_id;

  std::vector<float> m_dynamic;
  std::vector<float> m_fixed;
  //std::vector<float> m_dynamic;
  std::vector<float> m_tex;
  void caculate_norm_angle(glm::vec3& n, double& theta, double& phi);
  void init_fixed_data();
  void add_dynamic_data(
      glm::vec3& color
      , glm::vec3& scal
      , glm::vec3& rotate
      , glm::vec3& translate
  );
};
