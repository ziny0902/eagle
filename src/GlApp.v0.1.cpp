#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <GLPP/util.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gtk/gtk.h>
#include "GlApp.v0.1.h"

#define NUM_OF_COORDI   3
#define TICK_SPACING    0.5 

#define X_MIN   -10
#define Y_MIN   -10
#define Z_MIN   -3
#define X_MAX   10 
#define Y_MAX   10 
#define Z_MAX   5

GlApp3D::GlApp3D(IniManager& ini) :
  m_manager(10)
  , m_x_axis(ini, {0, 0.0, 0.0}, {X_MIN, X_MAX}, TICK_SPACING, ENUM_X_AXIS)
  , m_y_axis(ini, {0, 0.0, 0.0}, {Y_MIN, Y_MAX}, TICK_SPACING, ENUM_Y_AXIS)
  , m_z_axis(ini, {0, 0.0, 0.0}, {Z_MIN, Z_MAX}, TICK_SPACING, ENUM_Z_AXIS)
  , m_plot3d()
  , m_mesh(50, 50)
  , m_vector3d({0.0, 0.0, 0.0},{3.0, 3.2, 2.5})
  , m_projection(1)
  , m_rotate(glm::mat4(1))
  , m_model(glm::mat4(1))
  , m_figure("BOX(1.5 1.5 0.0, 2.0 2.0 1.0)")
  // , m_ch_table(ini)
  , m_gl_window(ini)
  // , m_text("abcdefghijklm\nnopqrstuvwxyz", m_manager, ini, m_ch_table)

{
  m_w = 600;
  m_h = 600;


  m_shader = m_manager.request_gl_shader_create("shader/vertex.glsl", "shader/fragment.glsl");

  std::shared_ptr<Gl::Shader> shader_ptr = m_manager.get_shader_from_shader_id(m_shader);

  shader_ptr->Bind();
  shader_ptr->SetUniformMatrix4fv("rotate", m_rotate);
  shader_ptr->SetUniformMatrix4fv("trans", m_model);
  shader_ptr->UnBind();
  m_vector3d.add_vector({-2.0, 2.0, 3}, {-1.0, 1.0, 2});
  m_vector3d.add_vector({-2.0, - 2.0, -3},{-1.0, - 1.0, -2});
  m_vector3d.add_vector({-2.0,  2.0, -3},{-1.0,  1.0, -2});
  m_vector3d.add_vector({-1.0,  1.0, -2}, {-2.0,  2.0, -3});
  m_vector3d.add_vector({2.0, 2.0, -3}, {1.0, 1.0, -2});
  m_vector3d.add_vector({2.0, -2.0, -3},{1.0, -1.0, -2});
  m_vector3d.add_vector({2.0, -2.0, 3},{1.0, -1.0, 2});
  m_vector3d.add_vector({-2.0, -2.0, 3},{-1.0, -1.0, 2});

  // m_text.set_pos(100, 100);

  ChangeLookAt(0.0, 0.0, 0.0);
}

GlApp3D::~GlApp3D()
{

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

	m_mesh_enabled = ini.get_boolean("GlApp", "mesh_enabled");
	m_plot3d_enabled = ini.get_boolean("GlApp", "plot3d_enabled");
	m_figure_enabled = ini.get_boolean("GlApp", "figure_enabled");
	m_vector_enabled = ini.get_boolean("GlApp", "vector_enabled");
// end initialization
	
	glm::vec3 (*plot3d_fn)(float) = nullptr;
	glm::vec3 (*figure_fn)(float) = nullptr;
	float (*mesh_fn)(float, float) = nullptr;

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
		plot3d_fn =  (glm::vec3(*)(float))dlsym(lib_handle, plot3d_fn_str->c_str());
	if(figure_fn_str != nullptr)
		figure_fn =  (glm::vec3(*)(float))dlsym(lib_handle, figure_fn_str->c_str());
	if(mesh_fn_str != nullptr)
		mesh_fn =  (float(*)(float, float))dlsym(lib_handle, mesh_fn_str->c_str());

	char *error;
	if ((error = dlerror()) != NULL)  
	{
		std::cout << error << std::endl;
		exit(-1);
	}
	dlclose(lib_handle);
}

	if(m_mesh_enabled && mesh_fn != nullptr){
		if(mesh_range != nullptr){
			m_mesh.regfunc(
				{(float)(*mesh_range)[0], (float)(*mesh_range)[1]}, 
				{(float)(*mesh_range)[2], (float)(*mesh_range)[3]}, 
				mesh_fn
			);
		}
		m_mesh.init_gl_buffer(m_manager, layout, m_shader);
	}
	else {
		m_mesh_enabled = false;
	}

	if(m_plot3d_enabled && plot3d_fn != nullptr && plot3d_range != nullptr) {
		m_plot3d.regfunc(plot3d_fn);
		m_plot3d.sampling(
			(float)(*plot3d_range)[0], 
			(float)(*plot3d_range)[1], 
			(float)(*plot3d_range)[2]
		);
		m_plot3d.init_gl_buffer(m_manager, layout, m_shader);
		// m_plot3d.activate_realtime();
	}
	else{
		m_plot3d_enabled = false;
	}

	if(m_vector_enabled) {
		m_vector3d.init_gl_buffer(m_manager, layout, m_shader);
	}

	if(m_figure_enabled && figure_fn != nullptr) {
		m_figure.init_gl_buffer(m_manager, layout, m_shader);
		m_figure.regfunc(figure_fn);
		m_figure.action_start();
	}
	else {
		m_figure_enabled = false;
	}

	ChangeLookAt(0.0, 0.0, 0.0);
}

void GlApp3D::ChangeLookAt(float x, float y, float z)
{
  m_view = glm::lookAt(glm::vec3(x, y, z), 
                   glm::vec3(0, 0, 0), 
                   glm::vec3(0.0, 1.0, 0.0));
  m_projection = glm::perspective(45.0f, 1.0f*m_w/m_h, 0.1f, 1000.0f);
  glm::mat4 mvp = m_projection * m_view * m_model;

  std::shared_ptr<Gl::Shader> shader_ptr = m_manager.get_shader_from_shader_id(m_shader);
  shader_ptr->Bind();
  shader_ptr->SetUniformMatrix4fv("u_MVP", mvp);
  shader_ptr->UnBind();
}

void GlApp3D::ModelRotate(float x, float y, float z)
{
  glm::mat4 rotate_x = glm::rotate(glm::mat4(1), glm::radians(x), glm::vec3(1.0, 0.0, 0.0));
  glm::mat4 rotate_y = glm::rotate(glm::mat4(1), glm::radians(y), glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 rotate_z = glm::rotate(glm::mat4(1), glm::radians(z), glm::vec3(0.0, 0.0, 1.0));

  m_rotate= rotate_z * rotate_y * rotate_x;
  glm::mat4 mvp = m_projection * m_view * m_model;
	glm::vec4 p = mvp * m_rotate *glm::vec4(2, 0, 0, 1);

  std::shared_ptr<Gl::Shader> shader_ptr = m_manager.get_shader_from_shader_id(m_shader);
  shader_ptr->Bind();
  shader_ptr->SetUniformMatrix4fv("rotate", m_rotate);
  shader_ptr->UnBind();
}

void GlApp3D::ModelTranslate(float x, float y, float z)
{
  m_model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
  m_projection = glm::perspective(45.0f, 1.0f*m_w/m_h, 0.01f, 100.0f);

  glm::mat4 mvp = m_projection * m_view * m_model;

  std::shared_ptr<Gl::Shader> shader_ptr = m_manager.get_shader_from_shader_id(m_shader);
  shader_ptr->Bind();
  shader_ptr->SetUniformMatrix4fv("u_MVP", mvp);
  shader_ptr->UnBind();
}

#include <GL/glu.h>
void GlApp3D::screen_2_world(float &x, float &y, float &z)
{
  glm::mat4 mvp = m_projection * m_view * m_model * m_rotate;
  glm::mat4 inv_mvp =glm::inverse(m_rotate)* glm::inverse(m_projection *m_view*m_model);

  glm::vec4 p;
#ifdef __DEBUG__
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
  // m_text.set_window_size(w, h);
  m_gl_window.set_window_size(w, h);
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  std::shared_ptr<Gl::Shader> shader_ptr = m_manager.get_shader_from_shader_id(m_shader);

  shader_ptr->Bind();

  shader_ptr->SetUniform4f(c_u_color, 0.8f, 0.3f, 0.8f, 1.0f); 

  shader_ptr->SetUniformMatrix4fv("trans", Identity);

  // Draw x, y, z axis 
  m_x_axis.Update(t_p, m_manager);
  m_y_axis.Update(t_p, m_manager);
  m_z_axis.Update(t_p, m_manager);

  if(m_mesh_enabled){
    m_mesh.Update(t_p, m_manager);
  }
  if(m_plot3d_enabled) {
    m_plot3d.Update(t_p, m_manager);
  }

  if(m_vector_enabled){
    shader_ptr->SetUniform4f(c_u_color, 0.3f, 0.3f, 0.5f, 1.0f); 
    m_vector3d.Update(t_p, m_manager);
  }

  if(m_figure_enabled) {
    m_figure.move(t_p, m_manager, "trans");
    m_figure.Update(t_p, m_manager);
  }

  shader_ptr->UnBind();
  // m_text.update();
  m_gl_window.update();

  glFlush();
}
void GlApp3D::display_pixel_info(int x, int y, std::string &s)
{
  // m_text.update_string(x, y, std::move(s));
  m_gl_window.update_coord_info(x, y, s);
}

void GlApp3D::mouse_release(int x, int y)
{
  m_gl_window.mouse_release(x, y);
}


