#version 330 core
layout (location = 0) in vec4 coord; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
		gl_Position = projection * vec4(coord.xy, 0.0, 1.0);
		TexCoords = coord.zw;
}  
