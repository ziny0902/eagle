#include <iostream>
#include <GLPP/util.h>
#include <GLPP/gl_resource_manager.h>

using namespace Gl;

ResourceManager::ResourceManager(int cnt) :
m_vertArray(cnt)
{
  m_unibuffer_id = -1;
  for(int i = 0; i < cnt; i++){
    m_vertArray_alloc_table.push_back(false);
  }
  GLCall(glEnable(GL_DEPTH_TEST));
  GLCall(glEnable(GL_STENCIL_TEST));
  GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

}

void ResourceManager::clear()
{
  GLCall(glClearStencil(0)); 
  GLCall(glClear(
      GL_COLOR_BUFFER_BIT 
    | GL_DEPTH_BUFFER_BIT
    | GL_STENCIL_BUFFER_BIT
  )
  );
}

int ResourceManager::request_gl_alloc_vertexArray()
{
  int i;
  for(i = 0; i < m_vertArray_alloc_table.size(); i++){
    if(m_vertArray_alloc_table[i]==false) break;
  }
  if ( i == m_vertArray_alloc_table.size()) {
    std::cout << "Vertex Array is not available\n";
    return -1;
  }
  m_vertArray_alloc_table[i] = true;
  return i;
}

int ResourceManager::request_gl_shader_create(
    const char *v_f
    , const char *f_f
    , const char *debug_attr
                                              )
{
  std::shared_ptr<Gl::Shader>shader;
  if(debug_attr){
    shader
      = std::make_shared<Gl::Shader>(v_f, f_f, debug_attr);
  }
  else
    shader
      = std::make_shared<Gl::Shader>(v_f, f_f);
   m_shader_list.push_back(shader);
   return (m_shader_list.size() - 1);
}

int ResourceManager::request_gl_vbo_data(unsigned char* data, 
                    unsigned int data_bytes, 
                    unsigned int num_of_vertex, 
                    GLenum buffer_type,
                    const VertexBufferLayout& layout,
                    GLenum glprimitive_mode,
                    unsigned short shader_idx,
                    unsigned short vertexA_idx
                    )
{
  m_shader_list[shader_idx]->Bind();
  m_vertArray.Bind(vertexA_idx);

  std::shared_ptr<ResourceElement> element = std::make_shared<ResourceElement>();
  element->num_of_vertex = num_of_vertex;
  element->buffer_type = buffer_type;
  element->mode = glprimitive_mode;
  element->vbo = std::make_shared<Gl::GlBuffer>();
  element->shader = m_shader_list[shader_idx];
  element->vertexArray_idx = vertexA_idx;
  element->vbo->AddCapacity(buffer_type, data_bytes);
  if(data != NULL)
    element->vbo->AddData(data, data_bytes);

  if(buffer_type == GL_ARRAY_BUFFER)
    m_vertArray.AddBuffer( *(element->vbo), layout, vertexA_idx);

  m_vertArray.UnBind();
  m_shader_list[shader_idx]->UnBind();

  m_element_list.push_back(element);
  return (m_element_list.size() - 1);
}

void ResourceManager::replace_gl_vbo_data(
                    unsigned short element_id,
                    unsigned char* data, 
                    unsigned int data_bytes,
                    unsigned int num_of_vertex
                    )
{
  std::shared_ptr<ResourceElement> element = m_element_list[element_id];
  m_vertArray.Bind(element->vertexArray_idx);

  element->vbo->SetGlBuffer(data, data_bytes, element->buffer_type);
  element->num_of_vertex = num_of_vertex;

  m_vertArray.UnBind();
}

bool ResourceManager::add_gl_vbo_data(
                    unsigned short element_id,
                    unsigned char* data, 
                    unsigned int data_bytes,
                    unsigned int num_of_vertex
                    )
{
  if(element_id >= m_element_list.size()) {
    std::cout << __FILE__ << "(" << __LINE__ << ") : " << "Invaild element id \n";
    return false;
  }

  bool ret;
  std::shared_ptr<ResourceElement> element = m_element_list[element_id];

  m_vertArray.Bind(element->vertexArray_idx);
  ret = element->vbo->AddData(data, data_bytes);
  if( ret == false ){ 
    element->vbo->AddCapacity(
      element->buffer_type, 
      ((element->num_of_vertex * (int)(data_bytes/num_of_vertex)) + 1024*sizeof(float)*3)
    );
    m_vertArray.UnBind();
    return false;
  }
  m_vertArray.UnBind();
  element->num_of_vertex += num_of_vertex;
  return true;
}

std::shared_ptr<Gl::Shader> ResourceManager::get_shader_from_element_id(unsigned short element_id)
{
  if(element_id >= m_element_list.size()) {
    std::cout << __FILE__ << "(" << __LINE__ << ") : " << "Invaild element id \n";
    return nullptr;
  }
  std::shared_ptr<ResourceElement> element = m_element_list[element_id];
  return element->shader;

}

std::shared_ptr<Gl::Shader> ResourceManager::get_shader_from_shader_id(unsigned short shader_id)
{
  if(shader_id >= m_shader_list.size()) {
    std::cout << __FILE__ << "(" << __LINE__ << ") : " << "Invaild shader_id \n";
    return nullptr;
  }
  return m_shader_list[shader_id];
}

void ResourceManager::gl_window_update(
    unsigned short element_id
    , bool debug
                                       )

{
  if(element_id >= m_element_list.size()) {
    std::cout << __FILE__ << "(" << __LINE__ << ") : " << "Invaild element id \n";
    return ;
  }

  std::shared_ptr<ResourceElement> element = m_element_list[element_id];
  gl_window_update(element_id, 0, element->num_of_vertex, debug);

}

void ResourceManager::gl_window_update(
  unsigned short element_id
  , int offset
  , int num_of_vertex
  , bool debug
)
{
  if(num_of_vertex == 0) return;
  if(element_id >= m_element_list.size()) {
    std::cout << __FILE__ << "(" << __LINE__ << ") : " << "Invaild element id \n";
    return ;
  }

  std::shared_ptr<ResourceElement> element = m_element_list[element_id];
  m_vertArray.Bind(element->vertexArray_idx);
  element->vbo->Bind();

  GLCall(glStencilFunc(GL_ALWAYS, element_id, -1));

  if(element->buffer_type == GL_ARRAY_BUFFER)
  {
    GLCall(glDrawArrays(element->mode, 0+offset, num_of_vertex));
  }
  else if(element->buffer_type == GL_ELEMENT_ARRAY_BUFFER)
  {
    GLCall(glDrawElements(
      element->mode,
      num_of_vertex,
      GL_UNSIGNED_SHORT, (void*)(offset*sizeof(GLuint))
    ));
  }
  else {
    std::cout << __FILE__ << "(" << __LINE__ << ") : " << "Invalid buffer type \n";
  }

  if(debug)
  {
    GLuint tbo;
    GLCall(glGenBuffers(1, &tbo));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, tbo));
    GLCall(glBufferData(
        GL_ARRAY_BUFFER, num_of_vertex*4*sizeof(float), nullptr, GL_STATIC_READ));

    // Perform feedback transform
    glEnable(GL_RASTERIZER_DISCARD);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);

    GLenum primitive
        = Gl::primitive_render_to_feedback((GLenum )element->mode);

    glBeginTransformFeedback(primitive);

    GLCall(glDrawArrays(element->mode, 0+offset, num_of_vertex));

    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    // Fetch and print results
    float *feedback = new float[num_of_vertex*4];
    GLCall(glGetBufferSubData(
        GL_TRANSFORM_FEEDBACK_BUFFER
        , 0
        , num_of_vertex*4*sizeof(float), feedback
                              ));
    std::cout <<"num of vertex: " << num_of_vertex << "\n";
    for(int i=0; i < 4*num_of_vertex; i+=4)
    {
      std::cout << feedback[i]
                << " " << feedback[i+1]
                << " " << feedback[i+2]
                << " " << feedback[i+3] << "\n";
    }

    glDeleteBuffers(1, &tbo);
  }


  element->vbo->UnBind();
  m_vertArray.UnBind(); 
}

void ResourceManager::resource_bind(unsigned short element_id) 
{
  if(element_id >= m_element_list.size()) {
    std::cout << __FILE__ << "(" << __LINE__ << ") : " << "Invaild element id \n";
    return;
  }
  std::shared_ptr<ResourceElement> e = m_element_list[element_id];
  e->shader->Bind();
  m_vertArray.Bind(e->vertexArray_idx);
  e->vbo->Bind();
  
}
void ResourceManager::resource_unbind(unsigned short element_id) 
{
  if(element_id >= m_element_list.size()) {
    std::cout << __FILE__ << "(" << __LINE__ << ") : " << "Invaild element id \n";
    return;
  }
  std::shared_ptr<ResourceElement> e = m_element_list[element_id];

  e->vbo->UnBind();
  m_vertArray.UnBind();
  e->shader->UnBind();
  
}
