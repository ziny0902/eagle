#include <GLPP/glWidgetManager.h>

using namespace Gl;

GlWidgetManager::GlWidgetManager(IniManager& ini) :
  GlDrawableWidget(
    *(m_res_ptr = std::make_shared<ResourceManager>(10)), 
    ini,
    *(m_font_table = std::make_shared<Gl::GlCharacterTable>(ini))
  )
{
  is_hidden_window = true;
}

GlWidgetManager::GlWidgetManager(
  ResourceManager& res,
  IniManager& ini
)
  : GlDrawableWidget(
      res,
      ini,
      *(m_font_table = std::make_shared<Gl::GlCharacterTable>(ini))
    )
{
  m_res_ptr = nullptr;
  is_hidden_window = true;
}

void GlWidgetManager::move(const int x, const int y)
{
  GlWidget::move(x, y);
  for( GlWidget& widget : m_widgets)
  {
    int _x, _y;
    widget.get_position(_x, _y);
    widget.move(x+_x, y - _y);
  }
}

void GlWidgetManager::set_window_alignment(WidgetAliangment align)
{
  int offset = 0;
  int w, h;

  if(align == WIDGET_ALIGNMENT_TOP_RIGHT)
  {
    for( GlWidget& widget : m_widgets)
    {
      widget.get_size(w, h);
#ifdef __DEBUG__
      std::cout << "w : " << w << " h : " << h << std::endl;
      std::cout << "ww : " << m_ww << " wh : " << m_wh << std::endl;
#endif
      widget.move(m_ww - w, m_wh - (m_wh - offset));
      offset += h;
    }
  }
}

void GlWidgetManager::set_window_size(int w, int h)
{
  GlDrawableWidget::set_window_size(w, h);
  for( GlWidget& widget : m_widgets){
    widget.set_window_size(w, h);
  }
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
