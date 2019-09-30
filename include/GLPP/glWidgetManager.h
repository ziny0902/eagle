#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <GLPP/gl_resource_manager.h>
#include <GLPP/glWidget.h>
#include <util/ini.h>

namespace Gl
{

class GlWidgetManager : public GlWidget{
public:
  GlWidgetManager(ResourceManager& res, IniManager& ini);
  ~GlWidgetManager(){}
  std::shared_ptr<Gl::Shader> get_shader();
  void add(GlWidget &);
  // void on_mouse_button();
  void resize(const int w, const int h);
  void update();
  void resource_release(int id);

private:
  int m_shader_id;
  unsigned short m_element_bg;
  unsigned short m_element_fg;

  ResourceManager& m_res;

  std::vector<std::reference_wrapper<GlWidget>> m_widgets;
};
}
