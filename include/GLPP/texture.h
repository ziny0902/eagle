#pragma once

#include <string>
#include <epoxy/gl.h>

namespace Gl
{

class Texture
{
private:
  unsigned int m_RendererID;
public:
  Texture(const std::string& path);
  ~Texture();

  void Bind(unsigned int slot=0) const;
  void UnBind() const;
};

}


