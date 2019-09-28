#include <iostream>
#include <epoxy/gl.h>
#include <boost/geometry/algorithms/detail/assign_values.hpp>
#include "axis.3D.h"

void Axis3D::initialize_axis_data(
  IniManager &ini, 
  float *origin, 
  float *range, 
  float tickspacing, 
  AXIS_DIR dir
)
{
  point_3d s;
  point_3d e;

  float tickvalue;
  float ticksize = 0; 
  float arrow_head_len = 0;

  // initialize axis from file(graph3d.ini) 

  tickspacing = ini.get_double("GlApp", "tick_spacing");
  ticksize = ini.get_double("GlApp", "tick_size");
  arrow_head_len = ini.get_double("GlApp", "arrow_head_length");

  std::shared_ptr<std::vector<double>> v = nullptr;
  if(dir == ENUM_X_AXIS){
    v = ini.get_double_list("GlApp", "axis_x");
  }
  else if ( dir == ENUM_Y_AXIS){
    v = ini.get_double_list("GlApp", "axis_y");
  }
  else if ( dir == ENUM_Z_AXIS){
    v = ini.get_double_list("GlApp", "axis_z");
  }
  if(v != nullptr&&v->size() >= 2) 
  {
    range[0] = (*v)[0];
    range[1] = (*v)[1];
  }

  if(dir == ENUM_X_AXIS)
  {
    boost::geometry::assign_values(s, range[0], origin[1], origin[2]); 
    boost::geometry::assign_values(e, range[1], origin[1], origin[2]); 
    tickvalue = origin[0] + ((int)(range[0]/tickspacing))*tickspacing;
  }
  else if ( dir == ENUM_Y_AXIS)
  {
    boost::geometry::assign_values(s, origin[0], range[0], origin[2]); 
    boost::geometry::assign_values(e, origin[0], range[1], origin[2]); 
    tickvalue = origin[1] + ((int)(range[0]/tickspacing))*tickspacing;
  }
  else if ( dir == ENUM_Z_AXIS)
  {
    boost::geometry::assign_values(s, origin[0], origin[1], range[0]); 
    boost::geometry::assign_values(e, origin[0], origin[1], range[1]); 
    tickvalue = origin[2] + ((int)(range[0]/tickspacing))*tickspacing;
  } else {
    std::cout << __FILE__ << "(" << __LINE__ << ")" << std::endl;
  }
  m_axis.append(s);
  m_axis.append(e);

#ifdef __DEBUG__
std::cout << __FILE__ << "(" << __LINE__ << ")" << (range[1] - range[0]) << std::endl;
#endif
  m_axis.add_arrow_head(geo::GEO_S_E, arrow_head_len);

  for(; ; )
  {
    tickvalue = tickvalue + tickspacing; 
    if(tickvalue > range[1]) break;
    switch(dir) {
    case ENUM_X_AXIS:
      m_axis.append({tickvalue, origin[1], 0});
      m_axis.append({tickvalue, origin[1]+ticksize, 0});
      break;
    case ENUM_Y_AXIS:
      m_axis.append({origin[0], tickvalue, 0});
      m_axis.append({origin[0]+ticksize, tickvalue, 0});
      break;
    case ENUM_Z_AXIS:
      m_axis.append({origin[0], 0, tickvalue});
      m_axis.append({origin[0]+ticksize, 0, tickvalue});
      break;
    }
  }


#ifdef __DEBUG__
int cnt = m_axis.size()/sizeof(float);
std::cout << cnt << std::endl;
float *it = m_axis.getArray();
for(int i=0; i < cnt; i++)
  std::cout << it[i] << " ";
std::cout << std::endl;
#endif
}

Axis3D::Axis3D(IniManager &ini, float origin[3], float range[3], float tickspacing, AXIS_DIR dir)
{
  initialize_axis_data(ini, origin, range, tickspacing, dir);
}

Axis3D::Axis3D(IniManager &ini, std::vector<float> &&origin, std::vector<float> &&range, float tickspacing, AXIS_DIR dir)
{
  initialize_axis_data(ini, &origin[0], &range[0], tickspacing, dir);
}

void Axis3D::init_gl_buffer(
    Gl::ResourceManager &manager, 
    Gl::VertexBufferLayout& layout,
    int shader_id 
)
{
  int vertArray_id = manager.request_gl_alloc_vertexArray();

  gl_resource_id = manager.request_gl_vbo_data(
        (unsigned char *)getArray(),
        bytes(),
        size()/3,
        GL_ARRAY_BUFFER,
        layout,
        GL_LINES,
        shader_id,
        vertArray_id 
  );
}

void Axis3D::Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager)
{
  manager.gl_window_update(gl_resource_id);
}
