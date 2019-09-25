#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/adapted/boost_array.hpp>

BOOST_GEOMETRY_REGISTER_BOOST_ARRAY_CS(cs::cartesian)
typedef boost::geometry::model::d2::point_xy<double> point_type;
typedef boost::geometry::model::point<float, 3, boost::geometry::cs::cartesian> point_3d;

namespace geo{

enum GeoType{
  GEO_LINE_STRING,
  GEO_POLYGON,
  GEO_BOX,
  GEO_SEGMENT
};

enum ArrowDir{
  GEO_S_E,
  GEO_E_S
};

}

glm::vec3 rectangular_to_spherical(glm::vec3 s);
glm::vec3 spherical_to_rectangular(glm::vec3 s);
glm::vec3 rectangular_to_cylindrical(glm::vec3 s);
glm::vec3 cylindrical_to_rectangular(glm::vec3 s);
