#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 n;

out vec3 normal;
out vec3 fragPos;


uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
	
	// pozicija tocke koju sjencamo
	vec4 fragPosTemp = model * vec4(aPos, 1.0);

	normal = n;

    gl_Position = projection * view * fragPosTemp;

	vec3 fragPos = vec3(fragPosTemp);
}