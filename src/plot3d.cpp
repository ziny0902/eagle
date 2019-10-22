#include <Math/multivcalculus.h>
#include "plot3d.h"

Plot3D::Plot3D() 
{
  is_realtime = false;
  m_func_ptr = nullptr;
  m_t0 = steady_clock::now();
}

void Plot3D::sampling(float s, float e, float step)
{
  glm::dvec3 f_t;
  float tick = s;

  for( ; tick < e; tick +=step)
  {
    f_t = m_func_ptr(tick);
    m_data.push_back((float)f_t.x);
    m_data.push_back((float)f_t.y);
    m_data.push_back((float)f_t.z);
  }
}

std::shared_ptr<float[]> Plot3D::tangent_vector(float t)
{
  std::shared_ptr<float[]> T
      = MathHelper::get_tanget_v(m_func_ptr, t, 3);
  return T;
}

std::shared_ptr<float[]> Plot3D::normal_vector(float t)
{
  std::shared_ptr<float[]> N
      = MathHelper::get_normal_v(m_func_ptr, t, 3);
  return N;
}

std::shared_ptr<float[]> Plot3D::cal_func(float t)
{
  glm::dvec3 p = m_func_ptr(t);
  std::shared_ptr<float[]> ret
      (new float[3], std::default_delete<float[]>());
  ret[0] = (float)p.x;
  ret[1] = (float)p.y;
  ret[2] = (float)p.z;
  return ret;
}

void Plot3D::activate_realtime()
{
  is_realtime = true;
  m_t0 = steady_clock::now();
}

void Plot3D::deactivate_realtime()
{
  is_realtime = false;
}

void Plot3D::activate_sampler(steady_clock::time_point &t_c, Gl::ResourceManager& manager)
{
  if( is_realtime == false) {
    std::cout << "Plot is not a realtime mode.\n";
    return;
  }
  milliseconds t_d = duration_cast<milliseconds> (t_c - m_t0);
  float t = ((float)(t_d.count())/1000);
  glm::dvec3 f_t= m_func_ptr(t);

  int offset = m_data.size();
  m_data.push_back((float)f_t.x);
  m_data.push_back((float)f_t.y);
  m_data.push_back((float)f_t.z);

  bool ret = manager.add_gl_vbo_data(
    gl_resource_id,
    (unsigned char *) &m_data[offset],
    sizeof(float)*3,
    1
  );

  if(ret == false){ 
    manager.replace_gl_vbo_data(
      gl_resource_id,
      (unsigned char *)&m_data[0],
      m_data.size()*sizeof(float),
      m_data.size()/3
    );
    return;
  }
}

void Plot3D::init_gl_buffer(
  Gl::ResourceManager &manager, 
  Gl::VertexBufferLayout& layout,
  int shader_id
)
{
  int vertArray_id = manager.request_gl_alloc_vertexArray();
  int buffer_size = 0;
  int num_of_vertex = 0;
  unsigned char * buffer = (unsigned char *)&m_data[0];

  if(m_data.size() == 0 ) {
    buffer_size = 64*3*sizeof(float); 
    buffer = NULL;
    num_of_vertex = 0;
  }
  else {
    buffer_size = m_data.size()*sizeof(float);
    num_of_vertex = buffer_size/(3*sizeof(float));
  }


  gl_resource_id = manager.request_gl_vbo_data(
        buffer,
        buffer_size,
        num_of_vertex,
        GL_ARRAY_BUFFER,
        layout,
        GL_LINE_STRIP,
        shader_id,
        vertArray_id,
        is_realtime
        // false
        );
}

void Plot3D::Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager)
{
  if( is_realtime){
    activate_sampler(t_c, manager);
  }
  manager.gl_window_update(gl_resource_id);
}

#include <glm/gtc/constants.hpp>
double Plot3D::bsearch(glm::dvec3& target
               , double& t1
               , double& t0
               )
{
  double d1, d0, dc;
  double tc = (t1 + t0)/2;
  glm::dvec3 p1 = m_func_ptr(t1);
  glm::dvec3 p0 = m_func_ptr(t0);
  glm::dvec3 pc = m_func_ptr(tc);

  d1 = glm::distance(p1, target);
  d0 = glm::distance(p0, target);
  dc = glm::distance(pc, target);

  if(d1 < 0.2)
  {
    t0 = t1;
    return d1;
  }
  if(d0 < 0.2)
  {
    return d0;
  }

  if(dc < 0.2)
  {
    t0 = tc;
    return dc;
  }

  if(fabs(t1 - tc) < 0.1)
  {
    if(dc < d1 && dc < d0)
    {
      t0 = tc;
      return dc;
    }
    if( d0 < d1)
    {
      t1 = t0;
      t0 = t0 - (tc - t0);
      return bsearch(target, t1, t0);
    }
    t0 = t1;
    return d1;
  }

  glm::dvec2 _p0(p0.x, p0.y);
  glm::dvec2 _p1(p1.x, p1.y);
  glm::dvec2 _target(target.x, target.y);
  double dot
  = glm::dot(_p0, _target) /(glm::length(_p0)*glm::length(_target));
  double theta = acos(dot);
dot = glm::dot(_p1, _target) /(glm::length(_p1)*glm::length(_target));
  double alpha = acos (dot);
  dot = glm::dot(_p0, _p1)/(glm::length(_p0)*glm::length(_p1));
  double gama = acos(dot);
  bool not_between  = !( gama > alpha && gama > theta );

  if(not_between)
  {
    if(d1 < d0)
    {
      t0 = t1;
      t1 = t1 + (t1 - tc);
    }
    else{
      t1 = t0;
      t0 = t0 - (tc - t0);
    }
    return bsearch(target, t1, t0);
  }

  double argc = tc;
  double d_1_c = bsearch(target, t1, argc);
  double d_c_0 = bsearch(target, tc, t0);
  if(d_1_c < d_c_0){
    t0 = argc;
    return d_1_c;
  }
  else {
    t1 = tc;
    return d_c_0;
  }
}

double Plot3D::find_parameter(glm::dvec3&& coord)
{
  std::shared_ptr<float[]> T = tangent_vector(0);
  glm::dvec3 p0 = m_func_ptr(0);
  double t0 = 0;
  int cnt = 0;

  while(1)
  {
    double t_min = 0;
    double min_distance = std::numeric_limits<int>::max();
    double t1 = t0;
    double dx, dy, dz;
    dx = fabs(coord.x - p0.x);
    dy = fabs(coord.y - p0.y);
    dz = fabs(coord.z - p0.z);
    double drx = dx/sqrt(dx*dx + dy*dy + dz*dz);
    double dry = dy/sqrt(dx*dx + dy*dy + dz*dz);
    double drz = dz/sqrt(dx*dx + dy*dy + dz*dz);

    if(fabs(T[0]) > 1e-4)
    {
      t1 += drx*(coord.x - p0.x)/T[0];
    }
    if (fabs(T[1]) > 1e-4)
    {
      t1 += dry*(coord.y - p0.y)/T[1];
    }
    if(fabs(T[2]) > 1e-4)
    {
      t1 += drz*(coord.z - p0.z)/T[2];
    }

    double distance = bsearch(coord, t1, t0);
    if(distance < min_distance)
    {
      min_distance = distance;
      t_min = t0;
    }
    if(fabs(distance) < 0.2){
      return t0;
    }
    if(cnt > 100){
      std::cout << "Maximum iteration. "
                << min_distance << " \n";
      return t_min;
    }

    T = tangent_vector(t0);
    cnt ++;
  }
  return 0;
}

#ifdef __LINEAR_SEARCH__
double Plot3D::find_parameter(glm::dvec3&& coord)
{
  double c_distance = 0;
  double parameter;
  int len = m_data.size();
  for(int i = 0; i < len ; i+=3)
  {
    glm::dvec3 p(m_data[i], m_data[i+1], m_data[i+2]);
    double distance = glm::distance(coord, p);
    if(distance < c_distance)
    {
      c_distance = distance;
    }
  }
}
#endif


void Plot3D::set_highlight(
    Gl::ResourceManager& manager
    , Highlight& highlight
    , glm::vec3&& coord
    , std::string& msg
                   )
{
  msg.append("plot3d\n");
  highlight.set_highlight(manager
                          , NULL
                          , 0
                          , gl_resource_id
                          , gl_resource_id
                          , app_common::app_gl_object::plot3d
                          );

  double t = find_parameter(glm::dvec3(coord.x, coord.y, coord.z));
  glm::dvec3 ret = m_func_ptr(t);
  coord.x = (float) ret.x;
  coord.y = (float) ret.y;
  coord.z = (float) ret.z;
  highlight.set_selected_coord(manager, coord);
}
