#ifndef __GTK_WINDOWN_H__
#define __GTK_WINDOWN_H__
#include <gtkmm.h>
#include "GlApp.v0.1.h"
#include <util/ini.h>

int GtkAppMain(int argc , char **argv);

class GtkAppWindow : public Gtk::Window
{
public:
  GtkAppWindow();
  virtual ~GtkAppWindow();
  void on_glarea_realize();
  bool on_glarea_render(const Glib::RefPtr<Gdk::GLContext>& );
  bool on_timeout();
  void on_resize(int width, int height);
  void on_lookat_changed();
  void on_rotate_changed();
  void on_translate_changed();
  void on_lookat_reseted();
  void on_rotate_reseted();
  void on_translate_reseted();
  bool on_button(GdkEventButton* release_event);

protected:
  Glib::RefPtr<Gtk::Adjustment>  m_adjustment_lookat_x;
  Glib::RefPtr<Gtk::Adjustment>  m_adjustment_lookat_y;
  Glib::RefPtr<Gtk::Adjustment>  m_adjustment_lookat_z;

  Glib::RefPtr<Gtk::Adjustment>  m_adjustment_rotate_x;
  Glib::RefPtr<Gtk::Adjustment>  m_adjustment_rotate_y;
  Glib::RefPtr<Gtk::Adjustment>  m_adjustment_rotate_z;

  Glib::RefPtr<Gtk::Adjustment>  m_adjustment_translate_x;
  Glib::RefPtr<Gtk::Adjustment>  m_adjustment_translate_y;
  Glib::RefPtr<Gtk::Adjustment>  m_adjustment_translate_z;

  Gtk::Box box_container;
  Gtk::GLArea m_gl_area;
  Gtk::Scale m_Scale_x, m_Scale_y, m_Scale_z;
  Gtk::Scale m_rotate_x, m_rotate_y, m_rotate_z;
  std::shared_ptr<GlApp3D> m_gl_app;
  IniManager m_ini;

private:
  Gtk::Menu *m_menu_popup;
  Gtk::MenuItem *m_del_object_menu; 
  void create_pop_menu();
  void on_add_vector();
  void proc_pop_menu(GdkEventButton* event);
  void on_del_object();
  void register_sig_handler();
  bool on_key_event(GdkEventKey* key_event);
  bool on_mouse_motion(GdkEventMotion* motion_event);
  bool on_glarea_scroll(GdkEventScroll * e);
};

#endif
