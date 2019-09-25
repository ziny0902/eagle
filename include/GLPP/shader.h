#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Gl
{

class Shader
{
private:
  unsigned int m_RendererID;
  bool m_is_use;
  std::string m_vert_path, m_frag_path;;
  std::unordered_map<std::string, int> m_UniformLocationCache;
  std::unordered_map<std::string, int> m_AttribLocationCache;
public:
  Shader(const std::string& vertex, const std::string& fragment);
  ~Shader();

  void Bind() ;
  void UnBind();

  //Set uniforms
  void SetUniform1i(const std::string& name, int value);
  void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3 );
  void SetUniformMatrix4fv(const std::string& name, glm::mat4 &trans);
  int GetAttribLocation(std::string &&attr_name);
  void EnableAttrib(int id);
  void DisableAttrib(int id);
private:
  int GetUniformLocation(const std::string& name);
};

}
