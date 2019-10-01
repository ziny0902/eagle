#include <iostream>
#include "VectorAddDialog.h"

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
    ("vector_add_dialog-window.ui");
  Gtk::Button *Ok;
  Gtk::Button *Cancel;
  builder->get_widget("VectorAddDialog", m_dialog);
  builder->get_widget("ok", Ok);
  builder->get_widget("cancel", Cancel);
  builder->get_widget("x_coord", x_coord);
  builder->get_widget("y_coord", y_coord);
  builder->get_widget("z_coord", z_coord);

  Ok->signal_clicked().connect(sigc::mem_fun(*this,
    &VectorAddDialog::on_ok_clicked));
  Cancel->signal_clicked().connect(sigc::mem_fun(*this,
    &VectorAddDialog::on_cancel_clicked));

  m_dialog->show_all();
  int ret = m_dialog->run();

  Glib::ustring val = x_coord->get_text();
  x_str.append(val);
  val = y_coord->get_text();
  y_str.append(val);
  val = z_coord->get_text();
  z_str.append(val);

#ifdef __DEBUG__
  std::cout << __FILE__ 
  << " x: " << x_str
  << " y: " << y_str
  << " z: " << z_str << std::endl;
#endif

  delete m_dialog;
  return ret;
}
