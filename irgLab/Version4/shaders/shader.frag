#version 330 core
layout (location = 0) out vec4 FragColor;

//in vec3 color;

in FS {
    vec3 fColor;
};

void main()
{
    FragColor = vec4(fColor, 1.0);  
} 