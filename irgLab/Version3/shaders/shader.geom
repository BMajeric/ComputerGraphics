#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
//layout (line_strip, max_vertices = 4) out;

uniform vec3 eye;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() 
{
    // centar trokuta
    vec3 c = vec3((gl_in[0].gl_Position.x + gl_in[1].gl_Position.x + gl_in[2].gl_Position.x) / 3.0,
                  (gl_in[0].gl_Position.y + gl_in[1].gl_Position.y + gl_in[2].gl_Position.y) / 3.0,
                  (gl_in[0].gl_Position.z + gl_in[1].gl_Position.z + gl_in[2].gl_Position.z) / 3.0);

    // vektor od centra trokuta prema gledistu
    vec3 e = vec3(eye.xyz - c.xyz);

    // normala na ravninu u kojoj trokut lezi
    vec3 edge1 = vec3(gl_in[1].gl_Position.x - gl_in[0].gl_Position.x,
                      gl_in[1].gl_Position.y - gl_in[0].gl_Position.y,
                      gl_in[1].gl_Position.z - gl_in[0].gl_Position.z);
    vec3 edge2 = vec3(gl_in[2].gl_Position.x - gl_in[0].gl_Position.x,
                      gl_in[2].gl_Position.y - gl_in[0].gl_Position.y,
                      gl_in[2].gl_Position.z - gl_in[0].gl_Position.z);
    vec3 n = normalize(cross(edge1, edge2));

    // ako je skalarni produkt veci od 0 onda je prednji
    if (dot(n, e) > 0) {
        for (int i = 0; i < 3; i++) {
            vec4 temp = projection * view * model * vec4(gl_in[i].gl_Position.xyz, 1.0);
	        if (temp.w > 0.0000001)
		        gl_Position = temp / temp.w;
	        else 
		        gl_Position = temp;
            EmitVertex();
        }
    }
    // inace je zadnji

    EndPrimitive();
}