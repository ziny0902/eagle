#pragma once
#include <gtkmm.h>
#include <glm/glm.hpp>

class Plane3dAddDig
{
 public:
  Plane3dAddDig();
  ~Plane3dAddDig();

  int run();

 private:
  Gtk::Dialog *m_dialog;
  glm::vec3 m_vec_1;
  glm::vec3 m_vec_2;
  glm::vec3 m_vec_c;
  glm::vec4 m_vec_color;

  void on_ok_clicked();
  void on_cancel_clicked();
};
