#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aCol;

//out vec3 color;

uniform mat4 view;

uniform mat4 projection;

uniform mat4 model;

void main()
{
	//color = vec3(0, 0, 0);
	vec4 temp = projection * view * model * vec4(aPos, 1.0);
	if (temp.w > 0.0000001)
		gl_Position = temp / temp.w;
	else 
		gl_Position = temp;
}