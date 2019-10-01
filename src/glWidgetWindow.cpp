#include <utility>
#include "glWidgetWindow.h"

glWidgetWindow::glWidgetWindow(IniManager& ini):
  Gl::GlWidgetManager(ini)
  , m_coord_info("Info", m_res, ini, m_ch_font)
{
  m_coord_info.set_pos(100, 100);
  add(m_coord_info);
}

void glWidgetWindow::update_coord_info(
  int x,
  int y,
  std::string& s
)
{
  m_coord_info.update_string(x, y, std::move(s));
  m_coord_info.show();
}

void glWidgetWindow::mouse_release(int x, int y)
{
  m_coord_info.hide();
}
