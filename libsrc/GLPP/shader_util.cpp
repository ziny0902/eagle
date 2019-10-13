#include <iostream>
#include <fstream>
#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
#include <GLPP/util.h>

unsigned int Gl::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id= glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource( id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if ( result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char *)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);

		std::cout << "Failed to compile " << 
		(type == GL_VERTEX_SHADER ? "vertex" : "fragment")
		<< "shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

bool Gl::read_shader_file(const std::string &fname, std::string &shader_str) 
{
	std::fstream f;

	std::string p_str;

	f.open(fname);
	if ( !f.is_open() ){
		std::cout << " can't open " <<  fname << std::endl;
		return false;
	}

	while ( std::getline(f, p_str) )
	{
		shader_str += p_str + "\n";
	}

	f.close();

	return true;
}

unsigned int Gl::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

unsigned int Gl::CreateDebugShader(
    const std::string& vertexShader
    , const std::string& fragmentShader
    , const std::string& attr
                              )
{
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));

  const char *varying = attr.c_str();

  GLCall(
      glTransformFeedbackVaryings(
          program,
          1,
          &varying,
          GL_INTERLEAVED_ATTRIBS )
         );

	GLCall(glLinkProgram(program));

	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

GLenum Gl::primitive_render_to_feedback(GLenum primitive)
{
  switch(primitive){
    case GL_TRIANGLES:
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN:
      return GL_TRIANGLES;
    case GL_POINTS:
      return GL_POINTS;
    case GL_LINES:
    case GL_LINE_LOOP:
    case GL_LINE_STRIP:
      return GL_LINES;
    default:
      return primitive;
  }
}
