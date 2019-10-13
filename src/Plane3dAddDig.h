#pragma once
#include <gtkmm.h>
#include <glm/glm.hpp>

class Plane3dAddDig
{
 public:
  Plane3dAddDig();
  ~Plane3dAddDig();
  inline glm::vec3& get_v1(){ return m_vec_1; }
  inline glm::vec3& get_v2(){ return m_vec_2; }
  inline glm::vec3& get_vc(){ return m_vec_c; }
  inline glm::vec4& get_color(){ return m_vec_color; }
  inline double get_width(){ return m_width; }
  inline double get_height(){ return m_height; }

  int run();

 private:
  Gtk::Dialog *m_dialog;
  glm::vec3 m_vec_1;
  glm::vec3 m_vec_2;
  glm::vec3 m_vec_c;
  glm::vec4 m_vec_color;
  double m_width;
  double m_height;

  void on_ok_clicked();
  void on_cancel_clicked();
};
