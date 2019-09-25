#version 330 core

layout(location = 0) in vec4 position; 
varying vec4 graph_coord;
varying vec4 norm_coord;

uniform mat4 trans;
uniform mat4 u_MVP;
uniform mat4 rotate;

void main()
{
  graph_coord = u_MVP * rotate * trans *  vec4(position.xyz, 1);
  norm_coord = graph_coord/graph_coord.a;
  gl_Position = norm_coord; 
};
