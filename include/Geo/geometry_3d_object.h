#pragma once

#include <string>
#include <vector>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/adapted/boost_array.hpp>
#include <Geo/geometry_common.h>

namespace geo{

class GeoModel3D {
public:
  GeoModel3D() {}
	GeoModel3D(std::string const & wkt);

	GeoModel3D(boost::geometry::model::linestring<point_3d> const & lstr);
	GeoModel3D(boost::geometry::model::polygon<point_3d> const & poly); 
	GeoModel3D(boost::geometry::model::box<point_3d> const & box);
	GeoModel3D(boost::geometry::model::segment<point_3d> const & seg);

  GeoType getType() { return m_type; }
  int getArray(double *arr);
  float *getArray() { return &m_model[0][0]; }
  void getVector(std::vector<float> & v);
  void append(point_3d const & p);
  void clear();
  void append(boost::geometry::model::segment<point_3d> const & seg);
  void add_arrow_head(ArrowDir, float);
  int size();

private:
  boost::geometry::model::linestring<boost::array<float, 3>> m_model;
  GeoType m_type;
};

};
