#version 330 core

layout(location = 0) in vec4 position; 
varying vec4 graph_coord;

layout (std140) uniform UniBlock1
{
uniform mat4 u_MVP;
uniform mat4 rotate;
};

void main()
{
  graph_coord = u_MVP * rotate * vec4(position.xyz, 1);
  gl_Position = graph_coord; 
};
