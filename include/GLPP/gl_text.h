#pragma once
#include <chrono>
#include <vector>
#include <string>
#include <GLPP/glWidget.h>
#include <util/ini.h>
#include <GLPP/gl_resource_manager.h>
#include <GLPP/glDrawableWidget.h>

using namespace std::chrono;

namespace Gl {

class GlTextObject : public Gl::GlDrawableWidget{ 
public:
  GlTextObject(
    std::string &&s,
    ResourceManager& res,
    IniManager& ini,
    Gl::GlCharacterTable &_ch_font
  );
  void update_string
  (
    int x,
    int y,
    std::string &&s
  );
  void update();
  int get_width();
  int get_height();
  void set_pos(int x, int y);
  void set_scale(float scale);
  // virtual void set_font_size(size_t size);

  // for othogonal projection.
  void set_window_size(
    int w, 
    int h
  );

private:
  /* GL Resource start*/
  int gl_resource_id;
  int m_shader;
  /* GL Resource end.*/
  
  int m_margin;
  int m_tx, m_ty;
  float m_scale;
  int m_f_size; // font size
  int m_tw, m_th; //text size in pixels
  std::string m_text;
  std::vector<float> m_points;
  std::shared_ptr<std::vector<double>> m_color;

  void init_vertex_data();
  void init();
  void init_gl_buffer();
};

}
