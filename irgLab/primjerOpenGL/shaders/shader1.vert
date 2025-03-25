#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

//   layout (location = ---> 0 <---) in vec3 aPos;
//glVertexAttribPointer(---> 0 <---, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//   layout (location = ---> 1 <---) in vec3 aCol;
//glVertexAttribPointer(---> 1 <---, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)((3 * sizeof(float)))); 

out vec3 color;

void main()
{
	color = aCol;
    gl_Position = vec4(aPos, 1.0);
}