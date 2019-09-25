#include "plot3d.h"

Plot3D::Plot3D() 
{
  is_realtime = false;
  m_func_ptr = nullptr;
  m_t0 = steady_clock::now();
}

void Plot3D::sampling(float s, float e, float step)
{
  glm::vec3 f_t;
  float tick = s;

  for( ; tick < e; tick +=step)
  {
    f_t = m_func_ptr(tick);
    m_data.push_back(f_t.x);
    m_data.push_back(f_t.y);
    m_data.push_back(f_t.z);
  }
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
  double t = ((double)(t_d.count())/1000);
  glm::vec3 f_t= m_func_ptr(t);
  if(f_t.y <= 0) deactivate_realtime();
  m_data.push_back(f_t.x);
  m_data.push_back(f_t.y);
  m_data.push_back(f_t.z);

  bool ret = manager.add_gl_vbo_data(
    gl_resource_id,
    (unsigned char *) (&f_t.x),
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

  if(m_data.size() == 0 ) {
    buffer_size = 256*sizeof(float);  
  }
  else {
    buffer_size = m_data.size()*sizeof(float);
  }

  gl_resource_id = manager.request_gl_vbo_data(
        (unsigned char *)&m_data[0],
        buffer_size,
        buffer_size/(3*sizeof(float)),
        GL_ARRAY_BUFFER,
        layout,
        GL_LINE_STRIP,
        shader_id,
        vertArray_id 
        );
}

void Plot3D::Update(steady_clock::time_point &t_c, Gl::ResourceManager& manager)
{
  if( is_realtime){
    activate_sampler(t_c, manager);
  }
  manager.gl_window_update(gl_resource_id);
}
