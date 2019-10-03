#pragma once
#include <glm/glm.hpp>
#include <gtkmm.h>

class VectorAddDialog 
{
public:
  VectorAddDialog(); 
  ~VectorAddDialog(){
    m_dialog = nullptr;
    sx_coord = nullptr;
    sy_coord = nullptr;
    sz_coord = nullptr;
    ex_coord = nullptr;
    ey_coord = nullptr;
    ez_coord = nullptr;
  }
  int run();
  glm::vec3 get_sv() { return m_s_v; }
  glm::vec3 get_ev() { return m_e_v; }

private:
  Gtk::Dialog *m_dialog;
  Gtk::SpinButton *sx_coord;
  Gtk::SpinButton *sy_coord;
  Gtk::SpinButton *sz_coord;

  Gtk::SpinButton *ex_coord;
  Gtk::SpinButton *ey_coord;
  Gtk::SpinButton *ez_coord;

  std::string x_str;
  std::string y_str;
  std::string z_str;

  glm::vec3 m_s_v;
  glm::vec3 m_e_v;

  void on_ok_clicked();
  void on_cancel_clicked();
};

