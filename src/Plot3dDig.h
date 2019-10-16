#ifndef __PLOT_3D_DIG_H__
#define __PLOT_3D_DIG_H__
#include <gtkmm.h>
#include <glm/glm.hpp>

#define PLOT_DIG_T_FLAG 0x01
#define PLOT_DIG_N_FLAG 0x02
#define PLOT_DIG_B_FLAG 0x04

class Plot3dDig {

 public:
  Plot3dDig();
  ~Plot3dDig();
  int run();
  float get_parameter() {return m_parameter;}
  unsigned short get_selected() {return m_flag;}
  unsigned short get_selected_plane() {return m_sel_plane; }
  glm::vec4 get_plane_color() {return m_color;}

 private:
  Gtk::Dialog *m_dialog;
  float m_parameter;
  glm::vec4 m_color;
  unsigned short m_flag;
  unsigned short m_sel_plane;
  void on_ok_clicked();
  void on_cancel_clicked();

};
#endif
