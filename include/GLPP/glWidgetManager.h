#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <GLPP/gl_resource_manager.h>
#include <GLPP/glDrawableWidget.h>
#include <util/ini.h>

namespace Gl
{

class GlWidgetManager : public GlDrawableWidget{
public:
  GlWidgetManager(ResourceManager& res, IniManager& ini);
  ~GlWidgetManager(){}
  std::shared_ptr<Gl::Shader> get_shader();
  void add(GlWidget &);
  // void on_mouse_button();
  void update();
  void resource_release(int id);

private:
  std::vector<std::reference_wrapper<GlWidget>> m_widgets;
};
}
