#pragma once
#include <vector>
#include <chrono>
#include <Geo/geometry_3d_object.h>
#include <util/ini.h>
#include "gl_resource_manager.h"
#include "gl_drawing_obj.h"

using namespace std::chrono;

#ifndef __AXIS_DIR__
#define __AXIS_DIR__
enum AXIS_DIR {
  ENUM_X_AXIS,
  ENUM_Y_AXIS,
  ENUM_Z_AXIS
};
#endif

// Inheritance from Drawable_obj. 
// List of property from Inheritance.
//   - gl_resource_id.
//   - init_gl_buffer().
//   - Update().
//   - get_gl_resouce_id().
class Axis3D : Gl::Drawable_obj{
public:
  Axis3D(IniManager&, float origin[3], float range[3], float, AXIS_DIR);
  Axis3D(IniManager&, std::vector<float> &&origin, std::vector<float> &&range, float, AXIS_DIR);
  inline void setOffset(long offset) { m_offset = offset; }
  inline long getOffset() { return m_offset; }
  float * getArray() { return m_axis.getArray(); }
  int bytes() { return m_axis.size();}
  int size() { return m_axis.size()/sizeof(float); }

  // Instance of virtual function.
  void Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager);
  void init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id
  );

private:
  void initialize_axis_data(IniManager&, float *origin, float *range, float, AXIS_DIR dir);
  geo::GeoModel3D m_axis;
  long m_offset;
};
