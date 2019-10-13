#ifndef __GL_UTIL_H__
#define __GL_UTIL_H__
#include <string>
#include <iostream>
#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
#include <signal.h>

namespace Gl
{

extern unsigned int CompileShader(unsigned int type, const std::string& source);
extern bool read_shader_file(const std::string &fname, std::string &shader_str);
extern unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

extern unsigned int CreateDebugShader(
    const std::string& vertexShader
    , const std::string& fragmentShader
    , const std::string& attr
                                          );

#define ASSERT(x) if (!(x)) raise(SIGTRAP);
#define GLCall(x) Gl::GLClearError(); \
			x;\
			ASSERT(Gl::GLLogCall(#x, __FILE__, __LINE__))

inline void GLClearError()
{
	while(glGetError() != GL_NO_ERROR);
}

inline bool GLLogCall(const char* function, const char* file, int line)
{
	while( GLenum error = glGetError() )
	{
		std::cout << "[OpenGL Error] (" << error << "): " 
		<< function << " " 
		<< file << " " << ":" << line << std::endl;
		return false;
	}
	return true;
}
extern GLenum primitive_render_to_feedback(GLenum primitive);

}
#endif
