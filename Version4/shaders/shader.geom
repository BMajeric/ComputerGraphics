#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
//layout (line_strip, max_vertices = 4) out;

out FS {
    vec3 fColor;
} outputColor;

uniform vec3 eye;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

void main() 
{
    // centar trokuta
    vec3 c = vec3((gl_in[0].gl_Position.x + gl_in[1].gl_Position.x + gl_in[2].gl_Position.x) / 3.0,
                  (gl_in[0].gl_Position.y + gl_in[1].gl_Position.y + gl_in[2].gl_Position.y) / 3.0,
                  (gl_in[0].gl_Position.z + gl_in[1].gl_Position.z + gl_in[2].gl_Position.z) / 3.0);

    // vektor od centra trokuta prema gledistu
    vec3 e = vec3(normalize(eye.xyz - c.xyz));

    // vektor od centra trokuta prema izvoru svjetla
    vec3 lightDirection = vec3(normalize(lightPos.xyz - c.xyz));

    // normala na ravninu u kojoj trokut lezi
    vec3 edge1 = vec3(gl_in[1].gl_Position.x - gl_in[0].gl_Position.x,
                      gl_in[1].gl_Position.y - gl_in[0].gl_Position.y,
                      gl_in[1].gl_Position.z - gl_in[0].gl_Position.z);
    vec3 edge2 = vec3(gl_in[2].gl_Position.x - gl_in[0].gl_Position.x,
                      gl_in[2].gl_Position.y - gl_in[0].gl_Position.y,
                      gl_in[2].gl_Position.z - gl_in[0].gl_Position.z);
    vec3 n = normalize(cross(edge1, edge2));

    vec3 reflected = normalize(reflect(-lightDirection, n));

    vec3 ambientColor = ambient * lightColor;
    vec3 diffuseColor = lightColor * diffuse * max(dot(n, lightDirection), 0.0f);
    vec3 specularColor = lightColor * specular * pow(max(dot(e, reflected), 0.0f), shininess);

    outputColor.fColor = ambientColor + diffuseColor + specularColor;
    
    for (int i = 0; i < 3; i++) {
        gl_Position = projection * view * gl_in[i].gl_Position;
        EmitVertex();
    }
    

    EndPrimitive();
}