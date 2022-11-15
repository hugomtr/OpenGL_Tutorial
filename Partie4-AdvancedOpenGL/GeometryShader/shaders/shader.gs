#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT {
    vec2 texCoords;
} gs_in[]; // [] because geometry shader acts on a set of vertices as its input

out vec2 TexCoords; 

void main() {
    gl_Position = gl_in[0].gl_Position;
    TexCoords = gs_in[0].texCoords;
    EmitVertex();
    
    gl_Position = gl_in[1].gl_Position;
    TexCoords = gs_in[1].texCoords;
    EmitVertex();
    
    gl_Position = gl_in[2].gl_Position;
    TexCoords = gs_in[2].texCoords;
    EmitVertex();
    EndPrimitive();
}
