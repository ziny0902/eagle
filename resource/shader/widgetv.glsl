#version 330 core
layout (location = 0) in vec3 coord; // <vec2 pos, vec2 tex>

uniform mat4 projection;

void main()
{
  gl_Position = projection * vec4(coord, 1.0);
}  
