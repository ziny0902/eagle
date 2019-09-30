#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <GLPP/gl_resource_manager.h>
#include <GLPP/glWidget.h>
#include <util/ini.h>

namespace Gl
{

class GlDrawableWidget : public GlWidget{
public:
  GlDrawableWidget(ResourceManager& res, IniManager& ini);
  ~GlDrawableWidget(){}
  std::shared_ptr<Gl::Shader> get_shader();
  // void on_mouse_button();
  void resize(const int w, const int h);
  void update();

protected:
  ResourceManager& m_res;
  bool is_hidden_window;

private:
  int m_shader_id;
  unsigned short m_element_bg;
  unsigned short m_element_fg;

};
}
