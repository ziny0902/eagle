#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <GLPP/gl_resource_manager.h>
#include <GLPP/glWidget.h>
#include <GLPP/glFont.h>
#include <util/ini.h>
#include <GLPP/glFont.h>

namespace Gl
{

class GlDrawableWidget : public GlWidget{
public:
  GlDrawableWidget(
    ResourceManager& res,
    IniManager& ini,
    Gl::GlCharacterTable &_ch_font
  );
  ~GlDrawableWidget(){}
  std::shared_ptr<Gl::Shader> get_shader();
  // void on_mouse_button();
  void resize(const int w, const int h);
  void update();
  void hide() { m_is_hide = true; }
  void show() { m_is_hide = false; }

protected:
  ResourceManager& m_res;
  bool is_hidden_window;
  Gl::GlCharacterTable &m_ch_font;

private:
  int m_shader_id;
  unsigned short m_element_bg;
  unsigned short m_element_fg;

};
}
