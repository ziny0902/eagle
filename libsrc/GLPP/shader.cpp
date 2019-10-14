#include <GLPP/util.h>
#include <GLPP/shader.h>

using namespace Gl;

template<typename T> T ReturnError() {}
template<> int ReturnError<int>() {return -1;}
template<> void ReturnError<void>() {return ;}

#define RETURN_VALUE(T) ReturnError<T>();

#define CHECK_PROGRAM_VALID(T) \
  if(!m_is_use) {\
    std::cout << "program does not binded\n";\
    return RETURN_VALUE(T);\
  }

#include <util/file.h>
Shader::Shader(
    const std::string &v_path
    , const std::string &f_path
    , const std::string& debug_attr
               )
  : m_vert_path(v_path),
  m_frag_path(f_path), m_RendererID(0)
{
  std::string vertexShader;
  std::string fragmentShader;
  std::string full_path = get_fullpath(v_path);

  Gl::read_shader_file(get_fullpath(v_path), vertexShader);
  Gl::read_shader_file(get_fullpath(f_path), fragmentShader);
  if(debug_attr.size())
    m_RendererID
        = Gl::CreateDebugShader(vertexShader, fragmentShader, debug_attr);
  else
    m_RendererID = Gl::CreateShader(vertexShader, fragmentShader);
  m_is_use = false;
}

Shader::~Shader()
{
  glDeleteProgram(m_RendererID);
}

void Shader::Bind()
{
  m_is_use = true;
  GLCall(glUseProgram(m_RendererID));
} 

void Shader::UnBind()
{
  m_is_use = false;
  GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
  CHECK_PROGRAM_VALID(void) 
  int location_id;
  GLCall(location_id = GetUniformLocation(name));
  GLCall(glUniform1i( location_id, value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3 )
{
  CHECK_PROGRAM_VALID(void) 
  int location_id;
  GLCall(location_id = GetUniformLocation(name));
  GLCall(glUniform4f( location_id, v0, v1, v2, v3));
}

void Shader::SetUniform3fv(const std::string& name, int count, const float *value)
{
  CHECK_PROGRAM_VALID(void)
  int location_id;
  GLCall(location_id = GetUniformLocation(name));
  GLCall(glUniform3fv( location_id, count, value)); 
}

void Shader::SetUniform4fv(const std::string& name, int count, const float *value)
{
  CHECK_PROGRAM_VALID(void)
      int location_id;
  GLCall(location_id = GetUniformLocation(name));
  GLCall(glUniform4fv( location_id, count, value)); 
}

void Shader::GetUniformfv(const std::string& name, float *value)
{
  CHECK_PROGRAM_VALID(void)
      int location_id;
  GLCall(location_id = GetUniformLocation(name));
  GLCall(glGetUniformfv(m_RendererID, location_id, value));
}

int Shader::GetUniformLocation(const std::string& name)
{
  CHECK_PROGRAM_VALID(int) 
  if( m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    return m_UniformLocationCache[name];

  int location_id;
  GLCall(location_id = glGetUniformLocation(m_RendererID, name.c_str()));
  m_UniformLocationCache[name] = location_id;
  if( location_id == -1 )
    std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
  return location_id;
}

void Shader::SetUniformMatrix4fv(const std::string& name, glm::mat4 &trans)
{
  CHECK_PROGRAM_VALID(void) 
  GLCall(GLint uniTrans = glGetUniformLocation(m_RendererID, name.c_str()));
  GLCall(glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans)));
}

void Shader::UniformBlockBinding(const std::string& name, const int id)
{
  unsigned int index;
  GLCall(index = glGetUniformBlockIndex(m_RendererID, name.c_str()));
  GLCall(glUniformBlockBinding(m_RendererID, index, id));
}

int Shader::GetAttribLocation(std::string &&attr_name)
{
  CHECK_PROGRAM_VALID(int) 
  if( m_AttribLocationCache.find(attr_name) != m_AttribLocationCache.end())
    return m_AttribLocationCache[attr_name];

  int location_id;
  GLCall(location_id = glGetAttribLocation(m_RendererID, attr_name.c_str()));
  m_AttribLocationCache[attr_name] = location_id;
  return location_id;
}

void Shader::EnableAttrib(int id)
{
  CHECK_PROGRAM_VALID(void) 
  GLCall(glEnableVertexAttribArray(id));
}
void Shader::DisableAttrib(int id)
{
  CHECK_PROGRAM_VALID(void) 
  GLCall(glDisableVertexAttribArray(id));
}
