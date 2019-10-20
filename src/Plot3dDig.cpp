#include <iostream>
#include <util/file.h>
#include "Plot3dDig.h"

Plot3dDig::Plot3dDig()
    : m_color(0)
{
  m_dialog = nullptr;
  m_flag = 0x00;
  m_sel_plane = 0x00;
  m_parameter = 0.0;
}

Plot3dDig::~Plot3dDig()
{}

int Plot3dDig::run()
{
  Glib::RefPtr<Gtk::Builder>builder = Gtk::Builder::create_from_file
      (get_fullpath("plot3d_add_vector_component-window.ui"));
  builder->get_widget("add_tnb_frame", m_dialog);

  Gtk::Button *Ok;
  Gtk::Button *Cancel;
  builder->get_widget("button_ok", Ok);
  builder->get_widget("button_cancel", Cancel);
  Ok->signal_clicked().connect(
      sigc::mem_fun(*this , &Plot3dDig::on_ok_clicked));
  Cancel->signal_clicked().connect(
      sigc::mem_fun(*this, &Plot3dDig::on_cancel_clicked));

  Glib::RefPtr<Glib::Object> g_obj;
  g_obj = builder->get_object("adj_parameter");
  Glib::RefPtr<Gtk::Adjustment>adj_parameter
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);
  adj_parameter->set_value(m_parameter);

  m_dialog->show_all();
  int ret = m_dialog->run();

  m_parameter = adj_parameter->get_value();

  Gtk::CheckButton *check;
  builder->get_widget("sel_tangent", check);
  bool T = check->get_active();

  builder->get_widget("sel_normal", check);
  bool N = check->get_active();

  builder->get_widget("sel_binormal", check);
  bool B = check->get_active();

  builder->get_widget("sel_TB", check);
  bool TB = check->get_active();

  builder->get_widget("sel_TN", check);
  bool TN = check->get_active();

  builder->get_widget("sel_BN", check);
  bool BN = check->get_active();

  Gtk::ColorButton *color_button;
  builder->get_widget("plane_color", color_button);
  Gdk::RGBA color = color_button->get_rgba();
  m_color = glm::vec4(
      color.get_red()
      , color.get_green()
      , color.get_blue()
      , color.get_alpha()
                      );

  m_flag = 0x0;
  if(T) m_flag |= PLOT_DIG_T_FLAG;
  if(N) m_flag |= PLOT_DIG_N_FLAG;
  if(B) m_flag |= PLOT_DIG_B_FLAG;

  if(TB) m_sel_plane |= 0x01;
  if(TN) m_sel_plane |= 0x02;
  if(BN) m_sel_plane |= 0x04;

  delete m_dialog;
  m_dialog = nullptr;

  return ret;
}

void Plot3dDig::on_ok_clicked()
{
  m_dialog->response(Gtk::RESPONSE_OK);
}

void Plot3dDig::on_cancel_clicked()
{
  m_dialog->response(Gtk::RESPONSE_CANCEL);
}
