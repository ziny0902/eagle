
#pragma once
#include <vector>
#include <epoxy/gl.h>
#include <GLPP/util.h>
#include <GLPP/glBuffer.h>

#define MAX_VAO_ID  5

namespace Gl
{
struct VertexBufferElement
{
  unsigned int type;
  unsigned int count;
  unsigned char normalized;
  unsigned int attr_id;
  
  static unsigned int GetSizeOfType(unsigned int type)
  {
    switch(type) 
    {
      case GL_FLOAT:          return 4;
      case GL_UNSIGNED_INT:   return 4;
      case GL_UNSIGNED_BYTE:  return 1;
    }
    ASSERT(false);
    return 0;
  }
};

class VertexBufferLayout 
{
private:
  std::vector<VertexBufferElement> m_Elements;
  unsigned int m_Stride;
public:
  VertexBufferLayout()
    : m_Stride(0) {}

  inline void Clear() { m_Stride=0; m_Elements.clear(); }
  
  template<typename T>
  void Push(unsigned int count, unsigned int attr_id)
  {
    // static_assert(false);
  }

  inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
  inline unsigned int GetStride() const { return m_Stride; }
};
template<>
void VertexBufferLayout::Push<float> (unsigned int count, unsigned int attr_id);
template<>
void VertexBufferLayout::Push<unsigned int> (unsigned int count, unsigned int attr_id);
template<>
void VertexBufferLayout::Push<unsigned char> (unsigned int count, unsigned int attr_id);


class VertexArrays{
public:
  VertexArrays(unsigned int cnt = 1);
  ~VertexArrays();
  void Bind(unsigned int idx = 0) ;
  void UnBind(void) ;
  void AddBuffer(GlBuffer& , const VertexBufferLayout&,  unsigned int idx = 0);
private:
  unsigned int m_vaoID[MAX_VAO_ID];
  unsigned int m_cnt;
};

}
