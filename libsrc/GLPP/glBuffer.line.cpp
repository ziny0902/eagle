#include <GLPP/glBuffer.line.h>

using namespace Gl;

GlBuffer::GlBuffer(unsigned int cnt)
{
  GLCall(glGenBuffers(cnt, &m_RendererId));
  m_size = 0;
  m_data_cnt = 0;
  m_type = GL_ARRAY_BUFFER;
}

GlBuffer::GlBuffer(const void *data, unsigned int size, GLenum type)
{
  bool ret; 
  m_type = type;
  m_size = 0;
  m_data_cnt = 0;

  GLCall(glGenBuffers(1, &m_RendererId));
  GLCall(glBindBuffer(m_type, m_RendererId));
  if(ret == false) AddCapacity(type);
  ret = AddData( data, size );
}

GlBuffer::~GlBuffer()
{
  // GLCall(glDeleteBuffers(1, &m_RendererId));
}

void GlBuffer::SetGlBuffer(const void *data, unsigned int size, GLenum type)
{
  m_data_cnt = 0;
  m_size = 0;
  m_type = type;
  Bind();
  bool ret = AddData(data, size );
  if(ret == false) AddCapacity(type);
  ret = AddData(data, size );
}

void GlBuffer::SetGlSubBuffer(const void *data, unsigned int size, unsigned int offset)
{
  Bind();
  GLCall(glBufferSubData(m_type, offset, size, data ));
}

bool GlBuffer::AddData(const void *data, unsigned int size )
{
  if(m_size < (m_data_cnt + size)) {
    std::cout << "not enough buffer" << std::endl;
    return false;
  }
  Bind();
  GLCall(glBufferSubData(m_type, m_data_cnt, size, data ));
  m_data_cnt += size;
  return true;
}

#define DEFAULT_BUFFER_SIZE 100*sizeof(float)
void GlBuffer::AddCapacity(GLenum type)
{
  if( m_size == 0) m_size = DEFAULT_BUFFER_SIZE;
  else m_size *= 2;
  m_type = type;
  m_data_cnt = 0;
  Bind();
  GLCall(glBufferData(m_type, m_size, NULL, GL_DYNAMIC_DRAW));
}

void GlBuffer::Bind()
{
  GLCall(glBindBuffer(m_type, m_RendererId));
}

void GlBuffer::UnBind()
{
  GLCall(glBindBuffer(m_type, 0));
}

