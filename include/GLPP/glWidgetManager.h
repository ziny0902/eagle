#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <GLPP/gl_resource_manager.h>
#include <GLPP/glDrawableWidget.h>
#include <util/ini.h>
#include <GLPP/glFont.h>

namespace Gl
{
enum WidgetAliangment {
  WIDGET_ALIGNMENT_TOP_RIGHT
  , WIDGET_ALIGNMENT_TOP_LEFT
};

class GlWidgetManager : public GlDrawableWidget{
public:
  GlWidgetManager(IniManager& ini);
  GlWidgetManager(ResourceManager& res, IniManager& ini);
  ~GlWidgetManager(){}
  virtual void move(const int x, const int y);

  std::shared_ptr<Gl::Shader> get_shader();
  void add(GlWidget &);
  void set_window_size(int w, int h);
  void set_window_alignment(WidgetAliangment align);
  // void on_mouse_button();
  void update();
  void resource_release(int id);

private:
  std::vector<std::reference_wrapper<GlWidget>> m_widgets;
  std::shared_ptr<ResourceManager> m_res_ptr;
  std::shared_ptr<Gl::GlCharacterTable> m_font_table;
};

}
