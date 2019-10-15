#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/acceleration.hpp>
#include <boost/units/pow.hpp>
#include <boost/units/io.hpp>

using namespace boost::units;
using namespace boost::units::si;
const static quantity<acceleration>    G(9.8 * meter_per_second / si::seconds); // and a distance,

extern "C" glm::vec3 mesh_graphf(float x, float y)
{
  float z = std::sin(fabs(x)+fabs(y));
  return glm::vec3(x, y, z);
}

extern "C" glm::vec3 parametric_surface(float u, float v)
{
  float x = (2 + std::sin(v)) * std::cos(u);
  float y = (2 + std::sin(v)) * std::sin(u);
  float z = u + std::cos(v);
  return glm::vec3(x, y, z);
}

extern "C" glm::dvec3 plot_3df(double t)
{
  double x = (4+ std::cos(20*t))*std::cos(t);
  double y = (4+ std::cos(20*t))*std::sin(t);
  double z = sin(20*t); 
  return glm::dvec3(x, y, z);
}

extern "C" glm::vec3 projectilef(double t)
{
  static quantity<si::velocity> v_0 = 38 * meter_per_second;
  static quantity<plane_angle> alpha = (M_PI/2.2)*radians;
  quantity<si::time> _t = t * second;
  quantity<si::length> x =(v_0 * std::cos(alpha.value())) * _t;
  quantity<si::length> y = (v_0 * std::sin(alpha.value())) * _t - 0.5*G * _t * _t;
  return glm::vec3(x.value()/10.0, y.value()/10.0, 0);
}

extern "C" glm::vec3 hookes_law(float t)
{
  float m_angf = 3; 
  quantity<plane_angle> m_alpha = 0*radians;
  quantity<si::length> m_amp = 4 * meter;
  float x = (m_amp + m_amp  * std::cos(m_angf * t + m_alpha.value())).value();
#ifdef __HOOKES_LAW_DEBUG__
std::cout << x << "(" << t << ")" << std::endl;
#endif
  return glm::vec3(x, 0, 0);
}


// gravitational constant
#define G ((6.67 * std::pow(10, -11) * newton * pow<2>(meter) ) / (pow<2>(kilogram)))

// earth mass
#define M_e  (5.98*std::pow(10, 24) * kilogram)

// earth radious
#define R_e (6.38*std::pow(10, 6) * meter)

// gravitational acceleration
#define g (9.8 * meter_per_second_squared)

// liftoff thrust 
#define F_rocket (7607*std::pow(10,3)*newton)

// kg
#define LEFTOFF_MASS 549054

// kg/s
#define FUEL_PER_SECOND 1451


extern "C" glm::vec3 rocket_thrust(double t)
{
// rocket mass 
  static quantity<mass>  m =  (LEFTOFF_MASS * kilogram);
  static quantity<force> F_prev = F_rocket - m*g;

  quantity<si::time> _t = t * second;
  quantity<force> F_next = 0 * newton;
  quantity<acceleration> a = 0 * meter_per_second_squared;
  quantity<velocity> v = 0 * meter_per_second;

  quantity<length> d = R_e + (F_prev/m)*_t*_t ;
  quantity<force> F_gravity = m*(G*M_e)/(d*d);

  F_next = F_rocket - F_gravity;
  a = F_prev / m;
  quantity<length> r = a * (_t*_t);
  v = a * _t;
  F_prev = F_next;
  m = LEFTOFF_MASS*kilogram - (FUEL_PER_SECOND*_t.value())*kilogram;

#ifdef __DEBUG__
std::cout << "a : " << a << std::endl;
std::cout << "v : " << v << std::endl;
std::cout << "r : " << r << std::endl;
std::cout << "m : " << m << std::endl;
std::cout << "t : " << _t << std::endl;
#endif
  return glm::vec3(t/10, a.value(), 0);
}
