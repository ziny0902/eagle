#pragma once
#include <glm/glm.hpp>

class Plane3d : public Gl::Drawable_obj{
 public:
  Plane3d();
  ~Plane3d();
  void add_plane(
      glm::vec3& s
      , glm::vec3& e
      , glm::vec3& c
      , double w
      , double h
      , glm::vec4& color
                 );

 private:
  glm::vec3 caculate_norm_vector(glm::vec3& v1, glm::vec3& v2);
  void caculate_norm_angle(glm::vec3& n, double& theta, double& phi);
};
