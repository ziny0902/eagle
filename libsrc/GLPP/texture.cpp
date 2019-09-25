#include <GLPP/util.h>
#include <GLPP/texture.h>
#include <SOIL/SOIL.h>
// #include "stb_image.h"
#include <cstring>

using namespace Gl;

Texture::Texture(const std::string& path)
  : m_RendererID(0) 
{

  m_RendererID= SOIL_load_OGL_texture(path.c_str(), 4, m_RendererID, 
   SOIL_FLAG_INVERT_Y);

  if(m_RendererID == 0)
  {
    std::cout<<"SOIL loading error: " << SOIL_last_result() << std::endl;
  }

  GLCall(glBindTexture(GL_TEXTURE_2D, 0));

}

Texture::~Texture()
{
  GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
  GLCall(glActiveTexture(GL_TEXTURE0 + slot));
  GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::UnBind() const
{
  GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

