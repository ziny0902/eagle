#version 330 core

// layout(location = 0) out vec4 color;
varying vec4 norm_coord;
uniform vec4 u_Color;

void main()
{

  gl_FragColor =  vec4(u_Color);
};
