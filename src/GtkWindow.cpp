#include <iostream>
#include <epoxy/gl.h>
#include <string>
#include <gtkmm/application.h>
#include "GtkWindow.h"
#include "VectorAddDialog.h"
#include "Plane3dAddDig.h"
#include "Plot3dDig.h"

using std::cerr;
using std::endl;
using std::string;

void GtkAppWindow::register_sig_handler()
{
  // Add Event Mask.
  m_gl_area.add_events(
      Gdk::BUTTON_RELEASE_MASK
    | Gdk::BUTTON_PRESS_MASK 
    | Gdk::POINTER_MOTION_MASK 
    | Gdk::SCROLL_MASK 
  );
  this->add_events(
      Gdk::KEY_PRESS_MASK 
    | Gdk::KEY_RELEASE_MASK
  );

// Add signal handler
  m_adjustment_lookat_x->signal_value_changed().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_lookat_changed));
  m_adjustment_lookat_y->signal_value_changed().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_lookat_changed));
  m_adjustment_lookat_z->signal_value_changed().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_lookat_changed));

  m_adjustment_rotate_x->signal_value_changed().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_rotate_changed));
  m_adjustment_rotate_y->signal_value_changed().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_rotate_changed));
  m_adjustment_rotate_z->signal_value_changed().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_rotate_changed));

  m_adjustment_translate_x->signal_value_changed().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_translate_changed));
  m_adjustment_translate_y->signal_value_changed().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_translate_changed));
  m_adjustment_translate_z->signal_value_changed().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_translate_changed));


  m_gl_area.signal_render().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_glarea_render), true);
  m_gl_area.signal_realize().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_glarea_realize));
  m_gl_area.signal_resize().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_resize));
  m_gl_area.signal_button_release_event().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_button));
  m_gl_area.signal_button_press_event().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_button));

  m_gl_area.signal_scroll_event().connect(
      sigc::mem_fun(*this, &GtkAppWindow::on_glarea_scroll)
  );

  this->signal_key_press_event().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_key_event));
  this->signal_key_release_event().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_key_event));
}

static Gtk::Box *create_labeled_scale(const char * l, Gtk::Scale &s)
{
  Gtk::Label *label = Gtk::manage(new Gtk::Label(l));
  Gtk::Box *control_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
  Gtk::Separator *separator = Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_VERTICAL));
  control_box->pack_start(*label, false, false);
  control_box->pack_start(*separator, false, false);
  control_box->pack_start(s);
  return control_box;
}

void GtkAppWindow::on_add_plane()
{
#ifdef __DEBUG__
  std::cout << "on_add_plane" 
            << std::endl;
#endif

  Plane3dAddDig add_plane3d;
  int ret = add_plane3d.run();

  if(ret == Gtk::RESPONSE_CANCEL) return; 
  m_gl_area.make_current();
  glm::vec3& v1 = add_plane3d.get_v1();
  glm::vec3& v2 = add_plane3d.get_v2();
  glm::vec3& vc = add_plane3d.get_vc();
  glm::vec4& color= add_plane3d.get_color();
  double w = add_plane3d.get_width();
  double h = add_plane3d.get_height();
  std::cout << __FILE__ << "w: " << w << " h: " << h <<"\n";
  m_gl_app->add_plane(
      v1
      , v2
      , vc
      , w
      , h
      , color
                      );
  gtk_gl_area_queue_render(m_gl_area.gobj());
}

void GtkAppWindow::on_add_vector()
{
#ifdef __DEBUG__
  std::cout << "on_add_vector" 
  << std::endl;
#endif

  VectorAddDialog dig;

  int ret = dig.run();
  if(ret == Gtk::RESPONSE_CANCEL) return; 
  m_gl_area.make_current();
  m_gl_app->add_vector(dig.get_sv(), dig.get_ev());
  gtk_gl_area_queue_render(m_gl_area.gobj());

}

void GtkAppWindow::on_del_object()
{
  m_gl_area.make_current();
  m_gl_app->del_select_object();
  gtk_gl_area_queue_render(m_gl_area.gobj());
}

void GtkAppWindow::on_add_TNB_frame()
{
  Plot3dDig dig;
  int ret = dig.run();
  if(ret == Gtk::RESPONSE_CANCEL) return; 

  m_gl_area.make_current();
  m_gl_app->add_TNB_frame(dig.get_parameter()
                          , dig.get_selected()
                          , dig.get_selected_plane()
                          , dig.get_plane_color()
                          );
  gtk_gl_area_queue_render(m_gl_area.gobj());
}

void GtkAppWindow::create_pop_menu()
{
  m_menu_popup = new Gtk::Menu;
  // Create menu items
  Gtk::MenuItem *item = new Gtk::MenuItem("Add vector");
  item->signal_activate().connect(
    sigc::mem_fun(this, &GtkAppWindow::on_add_vector)
  );
  m_menu_popup->append(*item);

  item = new Gtk::MenuItem("Add Plane");
  item->signal_activate().connect(
      sigc::mem_fun(this, &GtkAppWindow::on_add_plane)
                                  );
  m_menu_popup->append(*item);

  m_del_object_menu= new Gtk::MenuItem("delete selected object");
  m_del_object_menu->signal_activate().connect(
    sigc::mem_fun(this, &GtkAppWindow::on_del_object)
  );
  m_menu_popup->append(*m_del_object_menu);

  item= new Gtk::MenuItem("Add TNB frame");
  item->signal_activate().connect(
      sigc::mem_fun(this, &GtkAppWindow::on_add_TNB_frame)
                                               );
  m_menu_popup->append(*item);

  m_menu_popup->show_all();
}


GtkAppWindow::GtkAppWindow() :
  box_container(Gtk::ORIENTATION_VERTICAL, 10),
  m_adjustment_lookat_x( Gtk::Adjustment::create(0, -10.0, 10.0, 0.1, 1.0) ),
  m_adjustment_lookat_y( Gtk::Adjustment::create(0, -10.0, 10.0, 0.1, 1.0) ),
  m_adjustment_lookat_z( Gtk::Adjustment::create(10.0, -10.0, 10.0, 0.1, 1.0) ),

  m_adjustment_rotate_x( Gtk::Adjustment::create(0, -180, 180, 1.0, 2.0) ),
  m_adjustment_rotate_y( Gtk::Adjustment::create(0, -180, 180, 1.0, 2.0) ),
  m_adjustment_rotate_z( Gtk::Adjustment::create(0, -180, 180, 1.0, 2.0) ),

  m_adjustment_translate_x( Gtk::Adjustment::create(0, -10, 10, 1.0, 2.0) ),
  m_adjustment_translate_y( Gtk::Adjustment::create(-4.6, -10, 10, 1.0, 2.0) ),
  m_adjustment_translate_z( Gtk::Adjustment::create(0, -10, 10, 1.0, 2.0) ),

  m_rotate_x(m_adjustment_rotate_x),
  m_rotate_y(m_adjustment_rotate_y),
  m_rotate_z(m_adjustment_rotate_z),

  m_Scale_x(m_adjustment_lookat_x),
  m_Scale_y(m_adjustment_lookat_y),
  m_Scale_z(m_adjustment_lookat_z),
  m_ini("config/graph3d.ini")
{
m_menu_popup = nullptr;
// scale value init from file(graph3d.ini) 

std::shared_ptr<std::vector<double>> v = m_ini.get_double_list("window", "lookat_x");
if(v != nullptr&&v->size() >= 3) 
  m_adjustment_lookat_x->configure((*v)[0], (*v)[1], (*v)[2], 0.1, 2, 0);
v = m_ini.get_double_list("window", "lookat_y");
if(v != nullptr&&v->size() >= 3) 
  m_adjustment_lookat_y->configure((*v)[0], (*v)[1], (*v)[2], 0.1, 2, 0);
v = m_ini.get_double_list("window", "lookat_z");
if(v != nullptr&&v->size() >= 3) 
  m_adjustment_lookat_z->configure((*v)[0], (*v)[1], (*v)[2], 0.1, 2, 0);

v = m_ini.get_double_list("window", "rotate_x");
if(v != nullptr&&v->size() >=  3) 
  m_adjustment_rotate_x->configure((*v)[0], (*v)[1], (*v)[2], 0.1, 2, 0);
v = m_ini.get_double_list("window", "rotate_y");
if(v != nullptr&&v->size() >= 3) 
  m_adjustment_rotate_y->configure((*v)[0], (*v)[1], (*v)[2], 0.1, 2, 0);
v = m_ini.get_double_list("window", "rotate_z");
if(v != nullptr&&v->size() >= 3) 
  m_adjustment_rotate_z->configure((*v)[0], (*v)[1], (*v)[2], 0.1, 2, 0);

v = m_ini.get_double_list("window", "translate_x");
if(v != nullptr&&v->size() >= 3) 
  m_adjustment_translate_x->configure((*v)[0], (*v)[1], (*v)[2], 0.1, 2, 0);
v = m_ini.get_double_list("window", "translate_y");
if(v != nullptr&&v->size() >= 3) 
  m_adjustment_translate_y->configure((*v)[0], (*v)[1], (*v)[2], 0.1, 2, 0);
v = m_ini.get_double_list("window", "translate_z");
if(v != nullptr&&v->size() >= 3) 
  m_adjustment_translate_z->configure((*v)[0], (*v)[1], (*v)[2], 0.1, 2, 0);
//

  set_default_size(600, 600);

  box_container.set_homogeneous(false);
  box_container.pack_start(m_gl_area);

// Create control layout.
//
// 'Looak At' control panel.
  Gtk::Button *button_reset = Gtk::manage(new Gtk::Button("Reset"));
  button_reset->signal_clicked().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_lookat_reseted));

  Gtk::Grid *control_grid = Gtk::manage(new Gtk::Grid());
  control_grid->set_row_homogeneous(true);
  control_grid->set_column_homogeneous(true);
  control_grid->attach(*(create_labeled_scale(" X ", m_Scale_x)), 0, 0);
  control_grid->attach(*(create_labeled_scale(" Y ", m_Scale_y)), 1, 0);
  control_grid->attach(*(create_labeled_scale(" Z ", m_Scale_z)), 2, 0);
  Gtk::Label *label = Gtk::manage(new Gtk::Label("Look At"));
  label->set_size_request(60);
  Gtk::Box *label_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
  Gtk::Separator *separator = Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_VERTICAL));
  separator->set_size_request(10);
  label_box->pack_start(*label, false, false);
  label_box->pack_start(*separator, false, false);
  label_box->pack_start(*button_reset, false, false);
  label_box->pack_start(*control_grid);
  box_container.pack_start(*label_box, false, false);

// 'Rotate' control panel.
  button_reset = Gtk::manage(new Gtk::Button("Reset"));
  button_reset->signal_clicked().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_rotate_reseted));

  control_grid = Gtk::manage(new Gtk::Grid());
  control_grid->set_row_homogeneous(true);
  control_grid->set_column_homogeneous(true);
  label_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
  control_grid->attach(*(create_labeled_scale(" X ", m_rotate_x)), 0, 0);
  control_grid->attach(*(create_labeled_scale(" Y ", m_rotate_y)), 1, 0);
  control_grid->attach(*(create_labeled_scale(" Z ", m_rotate_z)), 2, 0);
  label = Gtk::manage(new Gtk::Label("Rotae"));
  label->set_size_request(60);
  separator = Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_VERTICAL));
  separator->set_size_request(10);
  label_box->pack_start(*label, false, false);
  label_box->pack_start(*separator, false, false);
  label_box->pack_start(*button_reset, false, false);
  label_box->pack_start(*control_grid);
  box_container.pack_start(*label_box, false, false);

// 'Translate' control panel.
  button_reset = Gtk::manage(new Gtk::Button("Reset"));
  button_reset->signal_clicked().connect(sigc::mem_fun(*this,
  &GtkAppWindow::on_translate_reseted));

  control_grid = Gtk::manage(new Gtk::Grid());
  control_grid->set_row_homogeneous(true);
  control_grid->set_column_homogeneous(true);
  label_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
  Gtk::Scale *translate_x = Gtk::manage(new Gtk::Scale(m_adjustment_translate_x));
  Gtk::Scale *translate_y = Gtk::manage(new Gtk::Scale(m_adjustment_translate_y));
  Gtk::Scale *translate_z = Gtk::manage(new Gtk::Scale(m_adjustment_translate_z));
  control_grid->attach(*(create_labeled_scale(" X ", *translate_x)), 0, 0);
  control_grid->attach(*(create_labeled_scale(" Y ", *translate_y)), 1, 0);
  control_grid->attach(*(create_labeled_scale(" Z ", *translate_z)), 2, 0);
  label = Gtk::manage(new Gtk::Label("Translate"));
  label->set_size_request(60);
  separator = Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_VERTICAL));
  separator->set_size_request(10);
  label_box->pack_start(*label, false, false);
  label_box->pack_start(*separator, false, false);
  label_box->pack_start(*button_reset, false, false);
  label_box->pack_start(*control_grid);
  box_container.pack_start(*label_box, false, false);

// Add highest level container 
  add(box_container);

  register_sig_handler();

// openGL depth buffer enable
  m_gl_area.set_has_depth_buffer(true);
  m_gl_area.set_has_stencil_buffer(true);

  m_gl_app = nullptr;

  show_all_children();
}

GtkAppWindow::~GtkAppWindow()
{
  // properly free openGL resource
  m_gl_area.make_current();
  m_gl_app = nullptr;
} 

void GtkAppWindow::on_lookat_changed()
{
  float x = m_Scale_x.get_value();
  float y = m_Scale_y.get_value();
  float z = m_Scale_z.get_value();

  if(m_gl_app != nullptr) {
    m_gl_area.make_current();
    m_gl_app->ChangeLookAt(x, y, z);
    gtk_gl_area_queue_render(m_gl_area.gobj());
  }

#ifdef __DEBUG__
  std::cout << "(" << x << "," << y << "," << z << ")" <<std::endl;
#endif
}

void GtkAppWindow::on_lookat_reseted()
{
std::shared_ptr<std::vector<double>> v = m_ini.get_double_list("window", "lookat_x");
if(v != nullptr&&v->size() >= 3) 
  m_adjustment_lookat_x->set_value((*v)[0]);
v = m_ini.get_double_list("window", "lookat_y");
if(v != nullptr&&v->size() >= 3) 
  m_adjustment_lookat_y->set_value((*v)[0]);
v = m_ini.get_double_list("window", "lookat_z");
if(v != nullptr&&v->size() >= 3) 
  m_adjustment_lookat_z->set_value((*v)[0]);
}

void GtkAppWindow::on_rotate_changed()
{
  float x = m_rotate_x.get_value();
  float y = m_rotate_y.get_value();
  float z = m_rotate_z.get_value();

  if(m_gl_app != nullptr) {
    m_gl_area.make_current();
    m_gl_app->ModelRotate(x, y, z);
    gtk_gl_area_queue_render(m_gl_area.gobj());
  }

#ifdef __DEBUG__
  std::cout << "(" << x << "," << y << "," << z << ")" <<std::endl;
#endif
}

void GtkAppWindow::on_rotate_reseted()
{
  std::shared_ptr<std::vector<double>>v 
    = m_ini.get_double_list("window", "rotate_x");
  if(v != nullptr&&v->size() >=  3) 
    m_adjustment_rotate_x->set_value((*v)[0]);
  v = m_ini.get_double_list("window", "rotate_y");
  if(v != nullptr&&v->size() >=  3) 
    m_adjustment_rotate_y->set_value((*v)[0]);
  v = m_ini.get_double_list("window", "rotate_z");
  if(v != nullptr&&v->size() >=  3) 
    m_adjustment_rotate_z->set_value((*v)[0]);
}

void GtkAppWindow::on_translate_changed()
{
  float x = m_adjustment_translate_x->get_value();
  float y = m_adjustment_translate_y->get_value();
  float z = m_adjustment_translate_z->get_value();

  if(m_gl_app != nullptr) {
    m_gl_area.make_current();
    m_gl_app->ModelTranslate(x, y, z);
    gtk_gl_area_queue_render(m_gl_area.gobj());
  }

#ifdef __DEBUG__
  std::cout << "(" << x << "," << y << "," << z << ")" <<std::endl;
#endif
}

void GtkAppWindow::on_translate_reseted()
{
  std::shared_ptr<std::vector<double>>v 
    = m_ini.get_double_list("window", "translate_x");
  if(v != nullptr&&v->size() >= 3) 
    m_adjustment_translate_x->set_value((*v)[0]);
  v = m_ini.get_double_list("window", "translate_y");
  if(v != nullptr&&v->size() >= 3) 
    m_adjustment_translate_y->set_value((*v)[0]);
  v = m_ini.get_double_list("window", "translate_z");
  if(v != nullptr&&v->size() >= 3) 
    m_adjustment_translate_z->set_value((*v)[0]);
}

void GtkAppWindow::on_glarea_realize()
{
// Create OpenGL Application.
  m_gl_area.make_current();
  m_gl_app = std::make_shared<GlApp3D>(m_ini);
  m_gl_app->StartUp(m_ini);
  m_gl_app->set_window_size(m_gl_area.get_width(), m_gl_area.get_height());
  //Glib::signal_timeout().connect( sigc::mem_fun(*this, &GtkAppWindow::on_timeout), 33);
  on_translate_changed();
  on_rotate_changed();
  on_lookat_changed();
}

void GtkAppWindow::on_resize(int width, int height)
{
  m_gl_area.make_current();
  glViewport(0, 0, width, height);
  m_gl_app->set_window_size(m_gl_area.get_width(), m_gl_area.get_height());
  float x = m_Scale_x.get_value();
  float y = m_Scale_y.get_value();
  float z = m_Scale_z.get_value();
  m_gl_app->ChangeLookAt(x, y, z);
  gtk_gl_area_queue_render(m_gl_area.gobj());
}

bool GtkAppWindow::on_timeout()
{
  gtk_gl_area_queue_render(m_gl_area.gobj());
  return true;
}

bool GtkAppWindow::on_glarea_render(const Glib::RefPtr<Gdk::GLContext>& ctx)
{
  m_gl_app->Draw(); 
  return true;
}

void GtkAppWindow::proc_pop_menu(GdkEventButton* event)
{
  if(m_menu_popup == nullptr)
    create_pop_menu();
  if(m_gl_app->is_selected()){
    m_del_object_menu->set_sensitive(true);
  }
  else {
    m_del_object_menu->set_sensitive(false);
  }
  m_menu_popup->popup(event->button, event->time);
  return;
}

bool GtkAppWindow::on_button(GdkEventButton* release_event)
{
  if((release_event->type == GDK_BUTTON_PRESS) &&
    (release_event->button == 3))
  {
    proc_pop_menu(release_event);
    return true;
  }
  if(release_event->type == GDK_BUTTON_RELEASE) {
    m_gl_app->mouse_release(release_event->x, release_event->y);
    gtk_gl_area_queue_render(m_gl_area.gobj());
    return true;
  }

  m_gl_area.make_current();
  m_gl_area.attach_buffers();

  m_gl_app->display_pixel_info(
    release_event->x,
    release_event->y
  );

  gtk_gl_area_queue_render(m_gl_area.gobj());

  return true;
}

bool GtkAppWindow::on_key_event(GdkEventKey* key_event)
{
  static std::shared_ptr<sigc::connection> sigc = nullptr;
  if(
    key_event->keyval != GDK_KEY_Shift_L
  ) 
    return false;

  if(key_event->type == GDK_KEY_PRESS){
#ifdef __DEBUG__
    std::cout << "key press\n";
#endif
     Glib::RefPtr<Gdk::Cursor> cursor 
    = Gdk::Cursor::create(
        m_gl_area.get_display()
        // , "move"
        , Gdk::CursorType::HAND1
      // , Gdk::CursorType::CROSSHAIR 
    );
    m_gl_area.get_window()->set_cursor(cursor);

    if(sigc == nullptr){
    sigc = std::make_shared<sigc::connection>(
      m_gl_area.signal_motion_notify_event().connect(
      sigc::mem_fun(*this, &GtkAppWindow::on_mouse_motion))
    );
    }
  }

  if(key_event->type == GDK_KEY_RELEASE){
#ifdef __DEBUG__
    std::cout << "key release\n";
#endif
    m_gl_area.get_window()->set_cursor();
    if(sigc != nullptr)
      sigc->disconnect();
    sigc=nullptr;
  }

  return false;
}

bool GtkAppWindow::on_mouse_motion(GdkEventMotion* motion_event)
{

  static float x=-1, y=-1;
  float dx = motion_event->x - x;
  float dy = motion_event->y - y;
  x = motion_event->x;
  y = motion_event->y;

  if(std::fabs(dx) > 5 || std::fabs(dy) > 5){
   return true; 
  }

  if(fabs(dy) > std::fabs(dx))
    m_adjustment_rotate_x->set_value(
      m_adjustment_rotate_x->get_value() + dy
    );
  else
    m_adjustment_rotate_y->set_value(
      m_adjustment_rotate_y->get_value() + dx
    );

#ifdef __DEBUG__
std::cout << "(" << dx
<< " , " << dy << ")"
<<std::endl;
#endif
  return true;
}

bool GtkAppWindow::on_glarea_scroll(GdkEventScroll * e)
{
  if (e->direction == GDK_SCROLL_UP)
  {
    m_adjustment_translate_z->set_value(
      m_adjustment_translate_z->get_value() - 0.5 
    );
  }
  else if (e->direction == GDK_SCROLL_DOWN)
  {
    m_adjustment_translate_z->set_value(
      m_adjustment_translate_z->get_value() + 0.5 
    );
  }

  return true;
}

int GtkAppMain(int argc, char *argv[])
{
  auto app = Gtk::Application::create("org.gtkmm.example");

  GtkAppWindow window;

  return app->run(window, argc, argv);
}
