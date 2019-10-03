#pragma once
#include "util.h"

namespace Gl
{

class GlBuffer{
public:
  GlBuffer();
  GlBuffer(const void *data, unsigned int size, GLenum type);
  ~GlBuffer();
  void SetGlBuffer(const void *data, unsigned int size, GLenum type);
  void SetGlSubBuffer(const void *data, unsigned int size, unsigned int offset);
  void AddCapacity(GLenum type);
  void AddCapacity(GLenum type, int size);
  bool AddData(const void *data, unsigned int size );
  unsigned int BindBufferRange(
    unsigned int id,
    unsigned int size
  );
  void Bind();
  void UnBind();
  unsigned int GetCount() { return m_size / sizeof(GLuint); }

  GlBuffer &operator=(const GlBuffer& rhs) = delete;

private:
  unsigned int m_RendererId;
  GLenum m_type;
  unsigned int m_size;
  unsigned int m_data_cnt;
};


}
