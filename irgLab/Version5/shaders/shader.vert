#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 n;

out vec3 color;


uniform vec3 eye;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;


void main()
{
	
	// pozicija tocke koju sjencamo
	vec4 pointPos = model * vec4(aPos, 1.0);
	
	// vektor od centra trokuta prema gledistu
    vec3 e = vec3(normalize(eye.xyz - pointPos.xyz));

    // vektor od centra trokuta prema izvoru svjetla
    vec3 lightDirection = vec3(normalize(lightPos.xyz - pointPos.xyz));

	vec3 reflected = reflect(-lightDirection, n);

	vec3 ambientColor = ambient * lightColor;
    vec3 diffuseColor = lightColor * diffuse * max(dot(n, lightDirection), 0.0f);
    vec3 specularColor = lightColor * specular * pow(max(dot(e, reflected), 0.0f), shininess); 

	color = ambientColor + diffuseColor + specularColor;

    gl_Position = projection * view * pointPos;

}