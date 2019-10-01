#pragma once
#include <string>
#include <GLPP/gl_text.h>
#include <GLPP/glWidgetManager.h>

class glWidgetWindow : public Gl::GlWidgetManager{
public:
  glWidgetWindow(IniManager& ini);
  void update_coord_info(int x, int y, std::string& s);
  void mouse_release(int x, int y);

private:
  Gl::GlTextObject m_coord_info;
};
