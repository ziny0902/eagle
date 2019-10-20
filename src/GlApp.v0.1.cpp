#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <boost/format.hpp>
#include <gtk/gtk.h>
#include <GLPP/util.h>
#include <Math/multivcalculus.h>
#include "GlApp.v0.1.h"
#include "define_common.h"

#define NUM_OF_COORDI   3
#define TICK_SPACING    0.5 

#define X_MIN   -10
#define Y_MIN   -10
#define Z_MIN   -3
#define X_MAX   10 
#define Y_MAX   10 
#define Z_MAX   5

GlApp3D::GlApp3D(IniManager& ini) :
  m_manager(20)
  , m_uniBuffer(NULL, 0, GL_UNIFORM_BUFFER)
  , m_x_axis(ini, {0, 0.0, 0.0}, {X_MIN, X_MAX}, TICK_SPACING, ENUM_X_AXIS)
  , m_y_axis(ini, {0, 0.0, 0.0}, {Y_MIN, Y_MAX}, TICK_SPACING, ENUM_Y_AXIS)
  , m_z_axis(ini, {0, 0.0, 0.0}, {Z_MIN, Z_MAX}, TICK_SPACING, ENUM_Z_AXIS)
  , m_plot3d()
  , m_mesh(50, 50)
  , m_vector3d({0.0, 0.0, 0.0},{3.0, 3.2, 2.5})
  , m_figure("BOX(1.5 1.5 0.0, 2.0 2.0 1.0)")
  , m_gl_window(ini)
  , m_rotate(glm::mat4(1))
  , m_projection(1)
  , m_view(1)
  , m_model(glm::mat4(1))
{
  m_obj_enabled = 0x0000;
  m_w = 600;
  m_h = 600;

  m_shader 
    = m_manager.request_gl_shader_create(
      "shader/vertex.glsl", "shader/fragment.glsl"
    );
  m_mesh_shader 
    = m_manager.request_gl_shader_create(
      "shader/meshv.glsl", "shader/meshf.glsl"
    );

  m_uniBuffer_id = m_manager.alloc_unibuffer_id();

  std::shared_ptr<Gl::Shader> shader_ptr = m_manager.get_shader_from_shader_id(m_shader);
  shader_ptr->Bind();
  shader_ptr->UniformBlockBinding("UniBlock1", m_uniBuffer_id);
  shader_ptr->UnBind();
  shader_ptr = m_manager.get_shader_from_shader_id(m_mesh_shader);
  shader_ptr->Bind();
  shader_ptr->UniformBlockBinding("UniBlock1", m_uniBuffer_id);
  shader_ptr->UnBind();

  m_uniBuffer.BindBufferRange(m_uniBuffer_id, 2*sizeof(glm::mat4));
  m_uniBuffer.AddData(glm::value_ptr(m_model), sizeof(glm::mat4));
  m_uniBuffer.AddData( glm::value_ptr(m_rotate), sizeof(glm::mat4));

  ChangeLookAt(0.0, 0.0, 0.0);
}

GlApp3D::~GlApp3D()
{
}

static unsigned short set_enabled(unsigned short mask, bool enabled)
{
  if(enabled) return mask;
  return 0;
}

#include <dlfcn.h>
void GlApp3D::StartUp(IniManager &ini)
{
  std::shared_ptr<Gl::Shader> shader_ptr = m_manager.get_shader_from_shader_id(m_shader);

  shader_ptr->Bind();
  unsigned int position_id = shader_ptr->GetAttribLocation("position");
  shader_ptr->UnBind();

  Gl::VertexBufferLayout layout;
  layout.Push<float> (3, position_id);

  m_x_axis.init_gl_buffer(m_manager, layout, m_shader);
  m_y_axis.init_gl_buffer(m_manager, layout, m_shader);
  m_z_axis.init_gl_buffer(m_manager, layout, m_shader);
  m_highlight.init_gl_buffer(m_manager, layout, m_shader);

// initialize class from config file.
	const std::shared_ptr<std::string> lib_path_str = ini.get_string("GlApp", "graph_lib_path");
	const std::shared_ptr<std::string> plot3d_fn_str = ini.get_string("GlApp", "plot3d_func");
	const std::shared_ptr<std::string> figure_fn_str = ini.get_string("GlApp", "figure_func");
	const std::shared_ptr<std::string> mesh_fn_str =  ini.get_string("GlApp", "mesh_func");
	std::shared_ptr<std::vector<double>> mesh_range 
		= ini.get_double_list("GlApp", "mesh_range");
	if(mesh_range != nullptr && mesh_range->size() != 4)
	{
		mesh_range = nullptr;
	}
	std::shared_ptr<std::vector<double>> plot3d_range 
		= ini.get_double_list("GlApp", "plot3d_range");
	if(plot3d_range != nullptr && plot3d_range->size() != 3)
	{
		plot3d_range = nullptr;
	}

	bool mesh_enabled = ini.get_boolean("GlApp", "mesh_enabled");
	bool plot3d_enabled = ini.get_boolean("GlApp", "plot3d_enabled");
	bool figure_enabled = ini.get_boolean("GlApp", "figure_enabled");
	bool vector_enabled = ini.get_boolean("GlApp", "vector_enabled");
  m_obj_enabled |= set_enabled(app_common::MESH_ENABLED, mesh_enabled);
  m_obj_enabled |= set_enabled(app_common::PLOT3D_ENABLED, plot3d_enabled);
  m_obj_enabled |= set_enabled(app_common::FIGURE_ENABLED, figure_enabled);
  m_obj_enabled |= set_enabled(app_common::VECTOR_ENABLED, vector_enabled);

// end initialization

  plot_func plot3d_fn = nullptr;
	glm::vec3 (*figure_fn)(float) = nullptr;
  glm::dvec3 (*mesh_fn)(double, double) = nullptr;

// load dynamic function.
{
	if(lib_path_str == nullptr) {
		std::cout << "not found lib path value in config file\n";
		exit(-1);
	}
	void *lib_handle = dlopen(lib_path_str->c_str(), RTLD_LAZY);
	if (!lib_handle)
	{
		std::cout << "can't load libgraphf.so : " << dlerror() << std::endl;
		exit(-1);
	}

	if(plot3d_fn_str != nullptr)
		plot3d_fn =  (plot_func)dlsym(lib_handle, plot3d_fn_str->c_str());
	if(figure_fn_str != nullptr)
		figure_fn =  (glm::vec3(*)(float))dlsym(lib_handle, figure_fn_str->c_str());
	if(mesh_fn_str != nullptr)
		mesh_fn =  (glm::dvec3(*)(double, double))dlsym(lib_handle, mesh_fn_str->c_str());

	char *error;
	if ((error = dlerror()) != NULL)  
	{
		std::cout << error << std::endl;
		exit(-1);
	}
	dlclose(lib_handle);
}

if((m_obj_enabled & app_common::MESH_ENABLED) && mesh_fn != nullptr){
		if(mesh_range != nullptr){
			m_mesh.regfunc(
				{(float)(*mesh_range)[0], (float)(*mesh_range)[1]}, 
				{(float)(*mesh_range)[2], (float)(*mesh_range)[3]}, 
				mesh_fn
			);
		}
		m_mesh.init_gl_buffer(m_manager, layout, m_mesh_shader);
	}
	else {
    m_obj_enabled &= ~app_common::MESH_ENABLED;
	}

	if( (m_obj_enabled & app_common::PLOT3D_ENABLED)
     && plot3d_fn != nullptr
     && plot3d_range != nullptr) {
    bool real_time = ini.get_boolean("GlApp", "real_time_enabled");
		m_plot3d.regfunc(plot3d_fn);
    if(real_time){
      m_plot3d.activate_realtime();
    }
    else
      m_plot3d.sampling(
        (float)(*plot3d_range)[0], 
        (float)(*plot3d_range)[1], 
        (float)(*plot3d_range)[2]
      );
		m_plot3d.init_gl_buffer(m_manager, layout, m_shader);
	}
	else{
    m_obj_enabled &= ~app_common::PLOT3D_ENABLED;
	}

	if(m_obj_enabled & app_common::VECTOR_ENABLED) {
		m_vector3d.init_gl_buffer(m_manager, layout, m_shader);
	}

	if((m_obj_enabled & app_common::FIGURE_ENABLED)
     && figure_fn != nullptr) {
		m_figure.init_gl_buffer(m_manager, layout, m_shader);
		m_figure.regfunc(figure_fn);
		m_figure.action_start();
	}
	else {
    m_obj_enabled &= ~app_common::FIGURE_ENABLED;
	}

  m_plane3d.init_gl_buffer(m_manager);
  m_plane3d.UniformBlockBinding(m_manager, "UniBlock1", m_uniBuffer_id);

	ChangeLookAt(0.0, 0.0, 0.0);
}

void GlApp3D::ChangeLookAt(float x, float y, float z)
{
  m_view = glm::lookAt(glm::vec3(x, y, z ), 
                   glm::vec3(0, 0, 0), 
                   glm::vec3(0.0, 1.0, 0.0));
  m_projection = glm::perspective(
    45.0f,
    1.0f*m_w/m_h,
    1.0f,
    40.0f 
  );
  glm::mat4 mvp = m_projection * m_view * m_model;

  std::shared_ptr<Gl::Shader> shader_ptr = m_manager.get_shader_from_shader_id(m_shader);

  m_uniBuffer.SetGlSubBuffer( 
    glm::value_ptr(mvp), sizeof(mvp), 0
  );
}

void GlApp3D::ModelRotate(float x, float y, float z)
{
  glm::mat4 rotate_x = glm::rotate(glm::mat4(1), glm::radians(x), glm::vec3(1.0, 0.0, 0.0));
  glm::mat4 rotate_y = glm::rotate(glm::mat4(1), glm::radians(y), glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 rotate_z = glm::rotate(glm::mat4(1), glm::radians(z), glm::vec3(0.0, 0.0, 1.0));

  m_rotate= rotate_z * rotate_y * rotate_x;

  m_uniBuffer.SetGlSubBuffer(
    glm::value_ptr(m_rotate),
    sizeof(m_rotate),
    sizeof(m_rotate)
  );
}

void GlApp3D::ModelTranslate(float x, float y, float z)
{
  m_model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
  // m_projection = glm::perspective(45.0f, 1.0f*m_w/m_h, 0.01f, 100.0f);

  glm::mat4 mvp = m_projection * m_view * m_model;

  m_uniBuffer.SetGlSubBuffer(
    glm::value_ptr(mvp),
    sizeof(mvp),
    0
  );

}

#include <GL/glu.h>
void GlApp3D::screen_2_world(float &x, float &y, float &z)
{
  glm::mat4 inv_mvp =glm::inverse(m_rotate)* glm::inverse(m_projection *m_view*m_model);

  glm::vec4 p;
#ifdef __DEBUG__
  glm::mat4 mvp = m_projection * m_view * m_model * m_rotate;
  p= mvp * glm::vec4(2, 0, -20, 1);
  p[0] = p[0]/p[3];
  p[1] = p[1]/p[3];
  p[2] = p[2]/p[3];
  p[3] = 1;

  glm::vec4 p_p = inv_mvp * p;
  p_p[0] = p_p[0]/p_p[3];
  p_p[1] = p_p[1]/p_p[3];
  p_p[2] = p_p[2]/p_p[3];
#endif

  p = glm::vec4(x, y, ((z -0.5)/0.5), 1);
  glm::vec4 position = inv_mvp *p; 
  x = position[0]/position[3];
  y = position[1]/position[3];
  z = position[2]/position[3];
}

void GlApp3D::set_window_size(int w, int h)
{
  m_w = w;
  m_h = h;
  m_gl_window.set_window_size(w, h);
  m_gl_window.set_window_alignment(Gl::WIDGET_ALIGNMENT_TOP_RIGHT);
}

static unsigned long getTick(steady_clock::time_point t_p)
{
  return  (duration_cast<milliseconds>( steady_clock::now() - t_p)).count(); 
}

void GlApp3D::Draw()
{
  static steady_clock::time_point t_p = steady_clock::now();
  static glm::mat4 Identity = glm::mat4(1.0f);

  if(  getTick(t_p) >= 12) {
    t_p = steady_clock::now();
  }

  m_manager.clear();

  std::shared_ptr<Gl::Shader> shader_ptr = m_manager.get_shader_from_shader_id(m_shader);

  shader_ptr->Bind();

  shader_ptr->SetUniformMatrix4fv("trans", Identity);
  shader_ptr->SetUniformMatrix4fv("scale", Identity);
  // Draw x, y, z axis 
  shader_ptr->SetUniform4f("u_Color", 1.0f, 0.0f, 0.0f, 1.0f); 
  m_x_axis.Update(t_p, m_manager);
  shader_ptr->SetUniform4f("u_Color", 0.0f, 1.0f, 0.0f, 1.0f); 
  m_y_axis.Update(t_p, m_manager);
  shader_ptr->SetUniform4f("u_Color", 0.0f, 0.0f, 1.0f, 1.0f); 
  m_z_axis.Update(t_p, m_manager);

  shader_ptr->SetUniform4f("u_Color", 0.0f, 0.5f, 1.0f, 1.0f); 

  if(m_obj_enabled & app_common::PLOT3D_ENABLED) {
    m_plot3d.Update(t_p, m_manager);
  }

  if(m_obj_enabled & app_common::VECTOR_ENABLED){
    m_vector3d.Update(t_p, m_manager);
  }

  if(m_obj_enabled & app_common::FIGURE_ENABLED) {
    m_figure.move(t_p, m_manager, "trans");
    m_figure.Update(t_p, m_manager);
  }

  shader_ptr->UnBind();

  if(m_obj_enabled & app_common::MESH_ENABLED){
    m_mesh.Update(t_p, m_manager);
  }

  m_highlight.Update(t_p, m_manager);

  m_plane3d.Update(t_p, m_manager);

  m_gl_window.update();

}

void GlApp3D::display_pixel_info(int x, int y)
{
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, (int *)&viewport[0]);

  float width = viewport[2];
  float height = viewport[3];
  width = width/2;
  height = height/2;

  float nx = (float)((x - width)/width);
  float ny = (float)((height - y)/height);
  float z = 0;

#ifdef __DEBUG__
  std::cout << "screen --> x: " << x << "; y: " << y <<std::endl;
  std::cout << "normalize --> x: " << nx << "; y: " << ny <<std::endl;
#endif

  float depths[9];
  unsigned int index[9];
  glReadPixels(
    x-1,
    viewport[3]+1 - y,
    3,
    3,
    GL_DEPTH_COMPONENT,
    GL_FLOAT,
    depths
  );

  glReadPixels(
    x-1,
    viewport[3]+1 - y,
    3,
    3,
    GL_STENCIL_INDEX,
    GL_UNSIGNED_INT,
    index
  );

  unsigned int id = 0;;
  z = 1;
  for(int i = 0; i < 9; i++){
    if(depths[i] != 1.0){
      z = depths[i];
      id = index[i];
      break;
    }
  }
  if(z == 1){
    m_highlight.clear();
    return ;
  }
  if(depths[5] != 1) {
    z = depths[5];
    id = index[5];
  }
#ifdef __DEBUG__
std::cout <<  "  depth: " << z 
<< " id: " << id
<<std::endl;
#endif

  screen_2_world(nx, ny, z);

  std::string msg; 
  post_pixel_sel(msg, nx, ny, z, id); 

#ifdef __DEBUG__
  std::cout << msg << std::endl;
#endif
  m_gl_window.update_coord_info(msg);
}

void GlApp3D::post_pixel_sel(
  std::string &msg,
  float x,
  float y,
  float& z,
  unsigned int id
)
{
  if(m_highlight.is_match(id)){
    // id = m_highlight.get_oid();
  }
  else
    m_highlight.clear();

  if (m_plot3d.is_match(id)) {
    m_plot3d.set_highlight(m_manager
                           , m_highlight
                           , glm::vec3(x, y, z)
                           , msg);
  }
  if (m_mesh.is_match(id)) {
    m_mesh.set_highlight(m_manager
                         , m_highlight
                         , glm::vec3(x, y, z)
                         , msg);
    float u, v;
    m_mesh.find_uv_parameter(glm::vec3(x, y, z), u, v);
    glm::vec3 ret = m_mesh.cal_func(u, v);
    glm::dvec3 gradiant = MathHelper::unit_normal(m_mesh.cal_func, u, v);
    m_vector3d.add_vector(m_manager
                          , {ret.x , ret.y , ret.z}
                          , { ret.x + (float)gradiant.x
                              , ret.y + (float)gradiant.y
                              , ret.z + (float)gradiant.z}
                          );

  }
  if (m_vector3d.is_match(id)) {
    m_vector3d.set_highlight(m_manager
                             , m_highlight
                             , glm::vec3(x, y, z)
                             , msg);
    return;
  }
  if (m_plane3d.is_match(id)) {
    glm::vec3 coord(x,y,z);
    m_plane3d.set_selected_plane(
        m_manager,
        m_highlight,
        coord);
  }

  boost::format fmt
      = boost::format("x: %s\ny: %s\nz: %s") % x % y % z;
  msg.append(fmt.str());
}

void GlApp3D::mouse_release(int x, int y)
{
  // m_gl_window.mouse_release(x, y);
}

void GlApp3D::add_vector(glm::vec3 s, glm::vec3 e)
{
#ifdef __DEBUG__
std::cout << "add_vector" << std::endl;
#endif
  m_vector3d.add_vector(
    m_manager,
    {s.x, s.y, s.z},
    {e.x, e.y, e.z}
  );
}

void GlApp3D::add_TNB_frame(float t
                    , unsigned short sel_vector
                    , unsigned short sel_plane
                    , glm::vec4 plane_color
                    )
{
  std::shared_ptr<float[]> T, N, B, p0;
  const long l = 1;
  T = m_plot3d.tangent_vector(t);
  N = m_plot3d.normal_vector(t);
  B = MathHelper::get_binormal_v(&T[0], &N[0]);
  p0 = m_plot3d.cal_func(t);

#ifdef __DEBUG__
  std::cout << "selected vector : " << sel_vector << "\n";
  std::cout << "selected plane : " << sel_plane << "\n";
#endif

  if(sel_vector & 0x01)
    m_vector3d.add_vector(
        m_manager,
        {p0[0], p0[1], p0[2]}
        , {p0[0]+T[0]*l, p0[1]+T[1]*l, p0[2] + T[2]*l}
    );
  if(sel_vector & 0x02)
    m_vector3d.add_vector(
        m_manager,
        {p0[0], p0[1], p0[2]}
        , {p0[0]+N[0]*l, p0[1]+N[1]*l, p0[2] + N[2]*l}
                          );
  if(sel_vector & 0x04)
    m_vector3d.add_vector(
        m_manager,
        {p0[0], p0[1], p0[2]}
        , {p0[0]+B[0]*l, p0[1]+B[1]*l, p0[2] + B[2]*l}
                          );
  glm::vec3 TV(T[0], T[1], T[2]);
  glm::vec3 BV(B[0], B[1], B[2]);
  glm::vec3 NV(N[0], N[1], N[2]);
  glm::vec3 C(p0[0], p0[1], p0[2]);
  if(sel_plane & 0x01)
    add_plane( TV, BV , C
              , 1, 1
              , plane_color
              );
  if(sel_plane & 0x02)
    add_plane( TV, NV , C
               , 1, 1
               , plane_color
               );
  if(sel_plane & 0x04)
    add_plane( BV, NV , C
               , 1, 1
               , plane_color
               );

}

void GlApp3D::add_plane(
    glm::vec3& s
    , glm::vec3& e
    , glm::vec3& c
    , double w
    , double h
    , glm::vec4& color
)
{
#ifdef __DEBUG__
  std::cout << "add plane" << std::endl;
#endif
  m_plane3d.add_plane(m_manager, s, e, c, w, h, color);
}

void GlApp3D::del_select_object()
{
  m_vector3d.delete_highlight_vector(m_manager
                                     , m_highlight.get_selected_coord());
  m_highlight.clear();
}

float GlApp3D::get_plot3d_selected_parameter()
{
  return m_plot3d.find_parameter(m_highlight.get_selected_coord());
}
