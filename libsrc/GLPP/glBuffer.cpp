#include <GLPP/glBuffer.h>

using namespace Gl;

GlBuffer::GlBuffer(bool queue_enabled)
{
  GLCall(glGenBuffers(1, &m_RendererId));
  m_size = 0;
  m_data_cnt = 0;
  m_queue_idx = 0;
  m_queue_enabled = queue_enabled;
  m_type = GL_ARRAY_BUFFER;
}


GlBuffer::GlBuffer(const void *data
                   , unsigned int size
                   , GLenum type
                   , bool queue_enabled
                   )
{
  m_type = type;
  m_size = 0;
  m_data_cnt = 0;
  m_queue_idx = 0;
  m_queue_enabled = queue_enabled;

  GLCall(glGenBuffers(1, &m_RendererId));
  GLCall(glBindBuffer(m_type, m_RendererId));
  AddCapacity(type);
  if(data != NULL)
    AddData( data, size );
}

GlBuffer::~GlBuffer()
{
  glDeleteBuffers(1, &m_RendererId);
}

void GlBuffer::SetGlBuffer(const void *data, unsigned int size, GLenum type)
{
  m_data_cnt = 0;
  m_type = type;
  Bind();
  bool ret = AddData(data, size );
  if(ret == false){ 
    AddCapacity(type, size);
    ret = AddData(data, size );
  }
}

void GlBuffer::SetGlSubBuffer(const void *data, unsigned int size, unsigned int offset)
{
  Bind();
  GLCall(glBufferSubData(m_type, offset, size, data ));
}

bool GlBuffer::AddData(const void *data, unsigned int size )
{
  Bind();
  if(m_queue_enabled)
  {
    if(m_size <= (m_data_cnt + size)) {
      int extra_bytes = (m_data_cnt+size) - m_size;
      if(extra_bytes > m_size){
        std::cout << "Error: Queue Overflow\n";
        return false;
      }
      GLCall(glBufferSubData(m_type, m_data_cnt, size-extra_bytes, data ));
      GLCall(glBufferSubData(m_type, 0, size, data ));
      m_data_cnt = m_queue_idx = size;
    }
    else{
      GLCall(glBufferSubData(m_type, m_data_cnt, size, data ));
      if(m_queue_idx == m_data_cnt && m_data_cnt != 0)
        m_queue_idx += size;
      m_data_cnt += size;
    }
    return true;
  }
  if(m_size < (m_data_cnt + size)) {
    std::cout << "("<< m_size << ")" << " not enough buffer" << std::endl;
    return false;
  }
  GLCall(glBufferSubData(m_type, m_data_cnt, size, data ));
  m_data_cnt += size;
  return true;
}

#define DEFAULT_BUFFER_SIZE 100*sizeof(float)
void GlBuffer::AddCapacity(GLenum type)
{
  m_size =  m_size + DEFAULT_BUFFER_SIZE;
  m_type = type;
  m_data_cnt = 0;
  Bind();
  GLCall(glBufferData(m_type, m_size, NULL, GL_DYNAMIC_DRAW));
}

void GlBuffer::AddCapacity(GLenum type, int size)
{
  m_size = size; 
  m_type = type;
  m_data_cnt = 0;
  Bind();
  GLCall(glBufferData(m_type, m_size, NULL, GL_DYNAMIC_DRAW));
}

unsigned int GlBuffer::BindBufferRange(
  unsigned int id,
  unsigned int size
)
{
  GLCall(glBindBufferRange(m_type, id, m_RendererId, m_data_cnt, size));
  return m_data_cnt;
}

void GlBuffer::Bind()
{
  GLCall(glBindBuffer(m_type, m_RendererId));
}

void GlBuffer::UnBind()
{
  GLCall(glBindBuffer(m_type, 0));
}

