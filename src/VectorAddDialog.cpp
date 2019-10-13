#include <iostream>
#include <cstdlib>
#include <util/file.h>
#include "VectorAddDialog.h"

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/string_cast.hpp>

void VectorAddDialog::on_ok_clicked()
{
  m_dialog->response(Gtk::RESPONSE_OK);
}

void VectorAddDialog::on_cancel_clicked()
{
  m_dialog->response(Gtk::RESPONSE_CANCEL);
}

VectorAddDialog::VectorAddDialog()
{
}

int VectorAddDialog::run()
{
  Glib::RefPtr<Gtk::Builder>builder = Gtk::Builder::create_from_file
    (get_fullpath("vector_add_dialog-window.ui"));
  Gtk::Button *Ok;
  Gtk::Button *Cancel;
  builder->get_widget("VectorAddDialog", m_dialog);
  builder->get_widget("ok", Ok);
  builder->get_widget("cancel", Cancel);
  builder->get_widget("sx_coord", sx_coord);
  builder->get_widget("sy_coord", sy_coord);
  builder->get_widget("sz_coord", sz_coord);
  builder->get_widget("ex_coord", ex_coord);
  builder->get_widget("ey_coord", ey_coord);
  builder->get_widget("ez_coord", ez_coord);

  Ok->signal_clicked().connect(sigc::mem_fun(*this,
    &VectorAddDialog::on_ok_clicked));
  Cancel->signal_clicked().connect(sigc::mem_fun(*this,
    &VectorAddDialog::on_cancel_clicked));

  m_dialog->show_all();
  int ret = m_dialog->run();

  m_s_v.x = std::stof(sx_coord->get_text());
  m_s_v.y = std::stof(sy_coord->get_text());
  m_s_v.z = std::stof(sz_coord->get_text());

  m_e_v.x = std::stof(ex_coord->get_text());
  m_e_v.y = std::stof(ey_coord->get_text());
  m_e_v.z = std::stof(ez_coord->get_text());

#ifdef __DEBUG__
  std::cout << __FILE__  << std::endl
  << " start: " << glm::to_string(m_s_v) 
  << std::endl
  << " end: " << glm::to_string(m_e_v) 
  << std::endl;
#endif

  delete m_dialog;
  return ret;
}
