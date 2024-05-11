#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

void main() {    
    vec2 p1 = vec2(gl_in[0].gl_Position.x, gl_in[0].gl_Position.y);
    vec2 p2 = vec2(gl_in[1].gl_Position.x, gl_in[1].gl_Position.y);
    vec2 p3 = vec2(gl_in[2].gl_Position.x, gl_in[2].gl_Position.y);
    
    if ((p2.y - p1.y)*(p3.x - p2.x) - (p3.y - p2.y)*(p2.x - p1.x) < 0.0f)
    {
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}