#pragma once
#include <gtkmm.h>

class VectorAddDialog 
{
public:
  VectorAddDialog(); 
  ~VectorAddDialog(){
    m_dialog = nullptr;
    x_coord = nullptr;
    y_coord = nullptr;
    z_coord = nullptr;
  }
  int run();

private:
  Gtk::Dialog *m_dialog;
  Gtk::SpinButton *x_coord;
  Gtk::SpinButton *y_coord;
  Gtk::SpinButton *z_coord;

  std::string x_str;
  std::string y_str;
  std::string z_str;

  void on_ok_clicked();
  void on_cancel_clicked();
};

