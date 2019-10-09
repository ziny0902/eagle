#include <iostream>
#include "Plane3dAddDig.h"


Plane3dAddDig::Plane3dAddDig()
{
  m_dialog = nullptr;
}

Plane3dAddDig::~Plane3dAddDig()
{
}

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/string_cast.hpp>
int Plane3dAddDig::run()
{
  Glib::RefPtr<Gtk::Builder>builder = Gtk::Builder::create_from_file
      ("plane3d_add_dialog-window.ui");
  builder->get_widget("Plane3dAddDialog", m_dialog);

  Gtk::Button *Ok;
  Gtk::Button *Cancel;
  Gtk::ColorButton *color_button;
  Glib::RefPtr<Glib::Object> g_obj;

  builder->get_widget("ok", Ok);
  builder->get_widget("cancel", Cancel);
  builder->get_widget("color", color_button);

  g_obj = builder->get_object("adj_x_vector1");
  Glib::RefPtr<Gtk::Adjustment>adj_v1_x
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);
  g_obj = builder->get_object("adj_y_vector1");
  Glib::RefPtr<Gtk::Adjustment>adj_v1_y
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);
  g_obj = builder->get_object("adj_z_vector1");
  Glib::RefPtr<Gtk::Adjustment>adj_v1_z
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);

  g_obj = builder->get_object("adj_x_vector2");
  Glib::RefPtr<Gtk::Adjustment>adj_v2_x
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);
  g_obj = builder->get_object("adj_y_vector2");
  Glib::RefPtr<Gtk::Adjustment>adj_v2_y
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);
  g_obj = builder->get_object("adj_z_vector2");
  Glib::RefPtr<Gtk::Adjustment>adj_v2_z
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);

  g_obj = builder->get_object("adj_x_center");
  Glib::RefPtr<Gtk::Adjustment>adj_vc_x
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);
  g_obj = builder->get_object("adj_y_center");
  Glib::RefPtr<Gtk::Adjustment>adj_vc_y
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);
  g_obj = builder->get_object("adj_z_center");
  Glib::RefPtr<Gtk::Adjustment>adj_vc_z
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);

  g_obj = builder->get_object("adj_width");
  Glib::RefPtr<Gtk::Adjustment>adj_width
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);
  g_obj = builder->get_object("adj_height");
  Glib::RefPtr<Gtk::Adjustment>adj_height
      = Glib::RefPtr<Gtk::Adjustment>::cast_static(g_obj);

  Ok->signal_clicked().connect(sigc::mem_fun(*this,
    &Plane3dAddDig::on_ok_clicked));
  Cancel->signal_clicked().connect(sigc::mem_fun(*this,
    &Plane3dAddDig::on_cancel_clicked));

  m_dialog->show_all();
  int ret = m_dialog->run();

  Gdk::RGBA color = color_button->get_rgba();

#ifdef __DEBUG__
  {
  std::cout << "vec1( " << adj_v1_x->get_value()
            << " " << adj_v1_y->get_value()
            << " " << adj_v1_z->get_value() << " )"
            << std::endl;

  std::cout << "vec2( " << adj_v2_x->get_value()
            << " " << adj_v2_y->get_value()
            << " " << adj_v2_z->get_value() << " )"
            << std::endl;

  std::cout << "center( " << adj_vc_x->get_value()
            << " " << adj_vc_y->get_value()
            << " " << adj_vc_z->get_value() << " )"
            << std::endl;

  std::cout << "( width: " << adj_width->get_value()
            << ", height: " << adj_height->get_value() << " )"
            << std::endl;
  std::cout << color.to_string() << std::endl;
  }
#endif

  m_vec_1 = glm::vec3(
      adj_v1_x->get_value(),
      adj_v1_y->get_value(),
      adj_v1_z->get_value()
                      );
  m_vec_2 = glm::vec3(
      adj_v2_x->get_value(),
      adj_v2_y->get_value(),
      adj_v2_z->get_value()
                      );
  m_vec_c = glm::vec3(
      adj_vc_x->get_value(),
      adj_vc_y->get_value(),
      adj_vc_z->get_value()
                      );
  m_vec_color = glm::vec4(
      color.get_red(),
      color.get_green(),
      color.get_blue(),
      color.get_alpha()
                      );
#ifdef __DEBUG__
  {
  std::cout << glm::to_string(m_vec_1) << std::endl;
  std::cout << glm::to_string(m_vec_2) << std::endl;
  std::cout << glm::to_string(m_vec_c) << std::endl;
  std::cout << glm::to_string(m_vec_color) << std::endl;
  }
#endif

  delete m_dialog;
  m_dialog = nullptr;

  std::cout << "ret: " << ret << std::endl;

  return ret;
}

void Plane3dAddDig::on_ok_clicked()
{
  m_dialog->response(Gtk::RESPONSE_OK);
}

void Plane3dAddDig::on_cancel_clicked()
{
  m_dialog->response(Gtk::RESPONSE_CANCEL);
}
