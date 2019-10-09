#include "plane3d.h"

Plane3d::Plane3d()
{}

Plane3d::~Plane3d()
{}

void Plane3d::add_plane(
    glm::vec3& s
    , glm::vec3& e
    , glm::vec3& c
    , double w
    , double h
    , glm::vec4& color
)
{
  double theta, phi;

  glm::vec3 norm = caculate_norm_vector(s, e);
  caculate_norm_angle(norm, theta, phi);
}

glm::vec3 Plan3d::caculate_norm_vector(glm::vec3& v1, glm::vec3& v2)
{}

void Plan3d::caculate_norm_angle(
    glm::vec3& n
    , double& theta
    , double& phi
)
{}
