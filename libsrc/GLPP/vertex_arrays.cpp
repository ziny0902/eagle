
#include <GLPP/vertex_arrays.h>

using namespace Gl;

template<>
void VertexBufferLayout::Push<float> (unsigned int count, unsigned int attr_id)
{
  m_Elements.push_back({ GL_FLOAT, count, GL_FALSE, attr_id});
  m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT)*count;
}

template<>
void VertexBufferLayout::Push<int> (unsigned int count, unsigned int attr_id)
{
  m_Elements.push_back({ GL_INT, count, GL_FALSE, attr_id});
  m_Stride += VertexBufferElement::GetSizeOfType(GL_INT)*count;
}

template<>
void VertexBufferLayout::Push<unsigned int> (unsigned int count, unsigned int attr_id) 
{
  m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE, attr_id});
  m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT)*count;
}

template<>
void VertexBufferLayout::Push<unsigned char> (unsigned int count, unsigned int attr_id)
{
  m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_FALSE, attr_id});
  m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE)*count;
}

VertexArrays::VertexArrays(unsigned int cnt)
{
  m_cnt = cnt;
  GLCall(glGenVertexArrays(cnt, m_vaoID));
}

VertexArrays::~VertexArrays()
{
  glDeleteVertexArrays(m_cnt, m_vaoID);
}
void VertexArrays::Bind(unsigned int idx)
{
  GLCall(glBindVertexArray(m_vaoID[idx]));
}
 
void VertexArrays::UnBind(void)
{
  GLCall(glBindVertexArray(0));
} 

void VertexArrays::AddBuffer( GlBuffer& gb, const VertexBufferLayout& layout, unsigned int idx)
{
  Bind(idx);
  gb.Bind();
  const auto& elements = layout.GetElements();
  unsigned int offset = 0;
  for ( unsigned int i = 0; i < elements.size(); i++)
  {
    const auto& element = elements[i];
    GLCall(glEnableVertexAttribArray(element.attr_id));
    GLCall(glVertexAttribPointer(element.attr_id, element.count, element.type,
      element.normalized, layout.GetStride(), (const void *)(uint64_t)offset));
    offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
  }
}
