#version 400 core
layout (location = 0) in float v_idx;
layout (location = 1) in float d_idx;

struct plane_info
{
  vec4 v_color;
  vec4 v_scale;
  vec4 v_rotate;
  vec4 v_translate;
};

layout (std140) uniform dynamic_block{
  plane_info value[8];
}dynamic_table;

layout (std140) uniform UniBlock1
{
uniform mat4 u_MVP;
uniform mat4 rotate;
};

uniform vec4 fixed_table[4];
varying vec4 coord;
varying vec4 color;

out vec4 debug_out;

mat4 scale( in float x, in float y) {
  return mat4( x, 0, 0, 0,
              0, y, 0, 0,
              0, 0, 1, 0,
              0, 0, 0, 1);
}

mat4 rotationY( in float angle ) {
	return mat4(	cos(angle),		0,		sin(angle),	0,
			 				0,		1.0,			 0,	0,
					-sin(angle),	0,		cos(angle),	0,
							0, 		0,				0,	1);
}

mat4 rotationX( in float angle ) {
  return mat4(	1.0,		0,			0,			0,
          0, 	cos(angle),	-sin(angle),		0,
          0, 	sin(angle),	 cos(angle),		0,
          0, 			0,			  0, 		1);
}

mat4 rotationZ( in float angle ) {
  return mat4(
          cos(angle),		-sin(angle),	0,	0,
          sin(angle),		cos(angle),		0,	0,
                   0,			      	0,	1,	0,
                   0,				      0,	0,	1
          );
}

void main()
{
  coord = scale(dynamic_table.value[int(d_idx)].v_scale.x,
                dynamic_table.value[int(d_idx)].v_scale.y)
          * fixed_table[int(v_idx)];

  coord
  = 
   rotationZ(dynamic_table.value[int(d_idx)].v_rotate.z) 
  * rotationX(dynamic_table.value[int(d_idx)].v_rotate.x)
  * coord; 

  coord
  = vec4(dynamic_table.value[int(d_idx)].v_translate.xyz + coord.xyz, 1);

  color = dynamic_table.value[int(d_idx)].v_color;

  //debug_out= dynamic_table.value[int(d_idx)].v_translate;
  //debug_out = dynamic_table.value[int(d_idx)].v_rotate;
  //debug_out = vec4(fixed_table[int(v_idx)], 1.0);
  //debug_out = vec4(fixed_table[1]);
  //debug_out = vec4(fixed_table[2], 1.0);
  //debug_out = vec4(fixed_table[3], 1.0);
  //debug_out = coord;
  //debug_out = color;
  debug_out = vec4(v_idx, d_idx, 0, 1);

  gl_Position = u_MVP * rotate * coord;
}  
