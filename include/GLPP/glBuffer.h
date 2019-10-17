#pragma once
#include "util.h"

namespace Gl
{

class GlBuffer{
public:
  GlBuffer(bool queue_enabled = false);
  GlBuffer(const void *data
           , unsigned int size
           , GLenum type
           , bool queue_enabled = false
           );
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
  unsigned int size() { return m_size; }
  unsigned int get_queue_index() { return m_queue_idx; }
  bool is_queue_enabled() {return m_queue_enabled;}

  GlBuffer &operator=(const GlBuffer& rhs) = delete;

private:
  unsigned int m_RendererId;
  GLenum m_type;
  unsigned int m_size;
  unsigned int m_data_cnt;
  unsigned int m_queue_idx;
  bool m_queue_enabled;
};


}
