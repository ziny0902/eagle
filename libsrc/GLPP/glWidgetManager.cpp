#include <GLPP/glWidgetManager.h>

using namespace Gl;

GlWidgetManager::GlWidgetManager(
  ResourceManager& res,
  IniManager& ini
)
  : GlDrawableWidget(res, ini)
{
  is_hidden_window = true;
}

void GlWidgetManager::add(GlWidget& _widget)
{
  
  m_widgets.push_back(_widget);
  _widget.set_parent(this);
  _widget.set_id(m_widgets.size() - 1);
}

void GlWidgetManager::update()
{
  GlDrawableWidget::update();

  for( GlWidget& w : m_widgets){
    if(w.is_hide()) continue;
    w.update();
  }

}

void GlWidgetManager::resource_release(int id)
{
  m_widgets.erase(m_widgets.begin()+id);
}
