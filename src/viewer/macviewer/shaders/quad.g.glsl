#version 330
layout(points) in;
layout(triangle_strip, max_vertices=4) out;
out vec2 st;

void main(void)
{
    gl_Position = vec4(-1.0,-1.0,0.0,1.0);
    st = vec2(0,0);
    EmitVertex();
    gl_Position = vec4(1.0,-1.0,0.0,1.0);
    st = vec2(1,0);
    EmitVertex();
    gl_Position = vec4(-1.0,1.0,0.0,1.0);
    st = vec2(0,1);
    EmitVertex();
    gl_Position = vec4(1.0,1.0,0.0,1.0);
    st = vec2(1,1);
    EmitVertex();
    EndPrimitive();
}
