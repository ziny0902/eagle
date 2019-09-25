#include <sstream>
#include <iostream>
#include <cmath>

#include <boost/geometry/geometry.hpp>
#include <Geo/geometry_3d_object.h>

using namespace geo;
using namespace boost::geometry;

static void box2linestring (
  boost::geometry::model::linestring<point_3d> &lstr
  ,boost::geometry::model::box<point_3d> const &box
)
{
  point_3d min_corner = box.min_corner();
  point_3d max_corner = box.max_corner();
  point_3d p;

  //front
  // min.x, min.y, min.z
  // max.x, min.y, min.z
  // max.x, min.y, max.z
  // min.x, min.y, max.z
  boost::geometry::assign(p, min_corner); 
  boost::geometry::append(lstr, p); 
  set<0>(p, get<0>(max_corner));
  boost::geometry::append(lstr, p); 
  set<1>(p, get<1>(max_corner));
  boost::geometry::append(lstr, p); 
  set<0>(p, get<0>(min_corner));
  boost::geometry::append(lstr, p); 

  // Back
  // min.x, max.y, min.z
  // max.x, max.y, min.z
  // max.x, max.y, max.z
  // min.x, max.y, max.z
  boost::geometry::assign(p, min_corner); 
  set<2>(p, get<2>(max_corner));
  boost::geometry::append(lstr, p); 
  set<0>(p, get<0>(max_corner));
  boost::geometry::append(lstr, p); 
  set<1>(p, get<1>(max_corner));
  boost::geometry::append(lstr, p); 
  set<0>(p, get<0>(min_corner));
  boost::geometry::append(lstr, p); 
}

GeoModel3D::GeoModel3D(std::string const & wkt)
{
  std::istringstream iss(wkt);
  std::string token;
  std::getline(iss, token, '(');

  if ((token == "LINESTRING")){
    m_type = GEO_LINE_STRING; 
    boost::geometry::model::linestring<point_3d> linestr;
    boost::geometry::read_wkt(wkt, linestr);
    boost::geometry::append(m_model, linestr); 
  }
  else if ((token == "POLYGON")){
    m_type = GEO_POLYGON;
    boost::geometry::model::polygon<point_3d> poly;
    boost::geometry::read_wkt(wkt, poly);
    boost::geometry::append(m_model, poly.outer()); 
  }
   else if ((token ==  "BOX")){
    m_type = GEO_BOX;
    boost::geometry::model::box<point_3d> box;
    boost::geometry::read_wkt(wkt, box);
    boost::geometry::model::linestring<point_3d> p;
    box2linestring(p, box); 
    boost::geometry::append(m_model, p); 
  }
   else if ((token == "SEGMENT")){
    m_type = GEO_SEGMENT;
    boost::geometry::model::segment<point_3d> seg;
    boost::geometry::read_wkt(wkt, seg);
    boost::geometry::assign(m_model, seg); 
  }
	else {
		std::cout << "Invalid wkt\n" ;
	}
}

GeoModel3D::GeoModel3D(boost::geometry::model::linestring<point_3d> const & lstr) 
{ 
  m_type = GEO_LINE_STRING;
  boost::geometry::append(m_model, lstr); 
}

GeoModel3D::GeoModel3D(boost::geometry::model::polygon<point_3d> const & poly) 
{ 
  m_type = GEO_POLYGON;
  boost::geometry::append(m_model, poly.outer()); 
}

GeoModel3D::GeoModel3D(boost::geometry::model::box<point_3d> const & box) 
{ 

  m_type = GEO_BOX;
  boost::geometry::model::linestring<point_3d> p;
  box2linestring(p, box);
  boost::geometry::append(m_model, p); 
}

GeoModel3D::GeoModel3D(boost::geometry::model::segment<point_3d> const & seg)
{ 
  m_type = GEO_SEGMENT;
  boost::geometry::assign(m_model, seg); 
}

namespace bg = boost::geometry;

int GeoModel3D::getArray(double *arr)
{
  int i = 0;
  for ( auto it = boost::begin(m_model); 
      it != boost::end(m_model); 
      ++it)
  {
    arr[i++] = bg::get<0>(*it);
    arr[i++] = bg::get<1>(*it);
    arr[i++] = bg::get<2>(*it);
  }
  return i;
}

void GeoModel3D::getVector(std::vector<float> & v)
{
  for ( auto it = boost::begin(m_model); 
      it != boost::end(m_model); 
      ++it)
  {
    v.push_back(bg::get<0>(*it));
    v.push_back(bg::get<1>(*it));
    v.push_back(bg::get<2>(*it));
  }
}

void GeoModel3D::append(point_3d const & p)
{
  m_type = GEO_LINE_STRING;
  boost::geometry::append(m_model, p); 
}

void GeoModel3D::append(boost::geometry::model::segment<point_3d> const & seg)
{
  m_type = GEO_LINE_STRING;
  boost::geometry::model::linestring<point_3d> lstr;
  boost::geometry::assign(lstr, seg); 
  boost::geometry::append(m_model, lstr); 
}


#include <boost/units/io.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/systems/angle/degrees.hpp>

static double ToRadians(double const && angle)
{
  return static_cast<
    boost::units::quantity
    <
      boost::units::si::plane_angle
    >
  > (angle * boost::units::degree::degrees).value();
}


void GeoModel3D::add_arrow_head(ArrowDir dir, float arrowLength)
{
  auto e = boost::end(m_model);
  auto b = boost::begin(m_model);
  --e;
  glm::vec3 e_p(e[0][0], e[0][1], e[0][2]);
// boost::geometry::assign_values(e_p, e[0][0], e[0][1], e[0][2]);


#ifdef __DEBUG__
std::cout << __FILE__ << "(" << __LINE__ << ") ";
std::cout << boost::geometry::wkt(*e) << std::endl;
#endif

  if( e == b)
    return;
  --e;

  glm::vec3 b_p( e[0][0], e[0][1], e[0][2]);
// boost::geometry::assign_values(b_p, e[0][0], e[0][1], e[0][2]);

#ifdef __DEBUG__
std::cout << boost::geometry::wkt(*e) << std::endl;
#endif
  // point_type b_p = *(--e);

  glm::vec3 v_dir;
  if( dir == GEO_S_E ){
    v_dir = e_p - b_p;
    
  }
  else {
    v_dir = b_p - e_p;
  }


  glm::vec3 s_coord = rectangular_to_spherical(v_dir);

  point_3d a_pt;

  double rad = ToRadians(20.0f); 
  set<0>(a_pt, (-arrowLength * std::cos(rad)));
  set<1>(a_pt, (arrowLength * std::sin(rad)));
  set<2>(a_pt, 0.0f);

#ifdef __DEBUG__
std::cout << __LINE__ << boost::geometry::wkt(a_pt) << std::endl;
std::cout << "phi : " << s_coord.z << std::endl;
std::cout << "theta : " << s_coord.y << std::endl;
#endif


  glm::mat4 rotate_z = glm::rotate(glm::mat4(1), s_coord.y, glm::vec3(0.0, 0.0, 1.0));
  glm::mat4 rotate_y = glm::rotate(glm::mat4(1), (float)(s_coord.z - M_PI/2), glm::vec3(0.0, 1.0, 0.0));

  point_3d  pt_0;
  glm::vec4 ret;
  
  if( dir == GEO_S_E ){
    set<0>(pt_0, e_p.x);
    set<1>(pt_0, e_p.y);
    set<2>(pt_0, e_p.z);
  } else {
    set<0>(pt_0, b_p.x);
    set<1>(pt_0, b_p.y);
    set<2>(pt_0, b_p.z);
  }
  glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(get<0>(pt_0), get<1>(pt_0) ,get<2>(pt_0) ));
  glm::mat4 transform = translate * rotate_z * rotate_y; 
  ret = transform * glm::vec4(get<0>(a_pt), get<1>(a_pt), get<2>(a_pt), 1.0);
  point_3d r_pt(ret.x, ret.y, ret.z);

#ifdef __DEBUG__
std::cout << boost::geometry::wkt(r_pt) << std::endl;
std::cout << boost::geometry::wkt(pt_0) << std::endl;
#endif

  append(r_pt);
  append(pt_0);
  append(pt_0);

#ifdef __DEBUG__
std::cout << boost::geometry::wkt(a_pt) << std::endl;
#endif
  ret = transform * glm::vec4(get<0>(a_pt), -1*get<1>(a_pt), get<2>(a_pt), 1.0);
  r_pt = point_3d (ret.x, ret.y, ret.z);

  append(r_pt);
}

int GeoModel3D::size()
{
  auto e = boost::end(m_model);
  auto b = boost::begin(m_model);
  return (e - b) * 3 * sizeof(float);
}
