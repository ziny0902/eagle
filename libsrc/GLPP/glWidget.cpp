#include <GLPP/glWidget.h>

GlWidget::GlWidget(
  std::shared_ptr<Gl::Shader> shader, 
  IniManager& ini,
  int x,
  int y,
  int w,
  int h
) :
m_va(1)
{
  m_x = x;
  m_y = y;
  m_width = w;
  m_height = h;

  data.append({0, 0, 0});
  data.append({0, (float)m_height, 0});
  data.append({(float)m_width, 0, 0});
  data.append({0, (float)m_height, 0});
  data.append({(float)m_width, 0, 0});
  data.append({(float)m_width, (float)m_height, 0});

  std::shared_ptr<std::string> vs
    = ini.get_string("Widget", "vertex_shader");
  std::shared_ptr<std::string> fs
    = ini.get_string("Widget", "fragment_shader");
  if(shader == nullptr) {
    m_shader = std::make_shared<Gl::Shader> (*vs ,*fs );
  }
  else {
    m_shader = shader;
  }

  m_shader->Bind();
  m_va.Bind(0);
  unsigned int coord = m_shader->GetAttribLocation("coord");
  m_vao.AddCapacity(GL_ARRAY_BUFFER, 6*3*sizeof(float)); 
  m_vao.AddData((char *)data.getArray(), data.size());
  Gl::VertexBufferLayout widget_layout;
  widget_layout.Push<float>(3, coord);
  m_va.AddBuffer(m_vao, widget_layout, 0);

  m_va.UnBind();
  m_shader->UnBind();

}

GlWidget::~GlWidget()
{
  m_shader = nullptr;
}

void GlWidget::set_window_pos(const int x, const int y)
{
  m_x = x;
  m_y = y;
}

void GlWidget::set_window_size(const int w, const int h)
{
  m_width = w;
  m_height = h;
}
void GlWidget::set_window_bg(glm::vec4& color)
{
  m_color = color;
}

void GlWidget::get_window_pos(int& x, int& y)
{
  x = m_x;
  y = m_y;
}

void GlWidget::get_window_size(int& w, int& h)
{
  w = m_width;
  h = m_height;
}

void GlWidget::Update()
{
  m_shader->Bind();
  m_va.Bind(0);
  m_vao.Bind();
  glm::mat4 trans 
    = glm::translate(glm::mat4(1), glm::vec3(m_x, m_y, 0));
  m_shader->SetUniformMatrix4fv("trans", trans);
  m_shader->SetUniform4f(
    "U_color",
    m_color.r,
    m_color.g,
    m_color.b,
    m_color.a
  );

  glDrawArrays(GL_TRIANGLES, 0, 6);

  m_vao.UnBind();
  m_va.UnBind();
  m_shader->UnBind();
}

void GlWidget::Drag(const int x, const int y)
{
}

void GlWidget::Clicked()
{
}
