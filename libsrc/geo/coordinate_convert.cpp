#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

glm::vec3 rectangular_to_spherical(glm::vec3 s)
{
  // rho = square root (x^2 + y^2 + z^2)
  double rho = std::sqrt(s.x*s.x + s.y*s.y + s.z*s.z);
  double phi = acos(s.z/rho);
  double theta;
#ifdef __DEBUG__
std::cout << "sin(phi) = " << sin(phi) << std::endl;
#endif
  if (sin(phi) == 0) theta = 0;
  else theta = acos(s.x/(rho*sin(phi)));
#ifdef __DEBUG__
  std::cout << "(" << rho << ", " << theta << ", " << phi << ")\n";
  std::cout << "(" << s.x << ", " << s.y<< ", " << s.z << ")\n";
#endif
  if(s.y < 0) {
    theta = 2*M_PI - theta;
  }

  return glm::vec3(rho, theta, phi);
}

glm::vec3 spherical_to_rectangular(glm::vec3 s)
{
  double x = s.x * std::sin(s.z)*std::cos(s.y);
  double y = s.x * std::sin(s.z)*std::sin(s.y);
  double z = s.x * std::cos(s.z);

#ifdef __DEBUG__
  std::cout << "(" << x << ", " << y << ", " << z << ")\n";
#endif
  return glm::vec3(x, y, z);
}

glm::vec3 rectangular_to_cylindrical(glm::vec3 s)
{
  double r = std::sqrt(s.x*s.x+ s.y*s.y);
  double theta = std::atan2(s.y, s.x);
#ifdef __DEBUG__
  std::cout << "(" << r << ", " << theta << ", " << s.z << ")\n";
#endif
  return glm::vec3(r, theta, s.z);
}

glm::vec3 cylindrical_to_rectangular(glm::vec3 s)
{
  double x = s.x * std::cos(s.y);
  double y = s.x * std::sin(s.y);
#ifdef __DEBUG__
  std::cout << "(" << x << ", " << y << ", " << s.z << ")\n";
#endif
  return glm::vec3(x, y, s.z);
}
