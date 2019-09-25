#version 330 core

// layout(location = 0) out vec4 color;
varying vec4 norm_coord;
uniform vec4 u_Color;
uniform vec4 u_Max;
uniform vec4 u_Min;

void main()
{

  float factor;

  if (gl_FrontFacing)
    factor = 1.0;
  else
    factor = 0.5;

  gl_FragColor =  vec4(u_Color.rgb/2+factor*norm_coord.rgb/2, 1);
};
