#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;

uniform vec3 eye;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

void main()
{

    // vektor od centra trokuta prema gledistu
    vec3 e = vec3(normalize(eye.xyz - fragPos.xyz));

    // vektor od centra trokuta prema izvoru svjetla
    vec3 lightDirection = vec3(normalize(lightPos.xyz - fragPos.xyz));

	vec3 reflected = reflect(-lightDirection, normal);

	vec3 ambientColor = ambient * lightColor;
    vec3 diffuseColor = lightColor * diffuse * max(dot(normal, lightDirection), 0.0f);
    vec3 specularColor = lightColor * specular * pow(max(dot(e, reflected), 0.0f), shininess); 

	vec3 color = ambientColor + diffuseColor + specularColor;

    FragColor = vec4(color, 1.0);  
} 