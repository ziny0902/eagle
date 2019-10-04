#version 330 core

// layout(location = 0) out vec4 color;
uniform vec4 u_Color;
varying vec4 graph_coord;

void main()
{

  float factor;

  if (gl_FrontFacing)
    factor = 1.0;
  else
    factor = 0.5;

  gl_FragColor =  vec4(u_Color.rgb/2+factor*graph_coord.rgb/(2*graph_coord.a), 1);
};
