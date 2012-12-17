#version 330
uniform mat4 modelViewProjectionMatrix;

layout(points) in;
layout(line_strip, max_vertices=2) out;
uniform float scale;
out vec4 f_color;
in vec3 dx[];

void main(void)
{
    gl_Position = modelViewProjectionMatrix * (gl_in[0].gl_Position);
    f_color = vec4(.2,.2,.2,1.0);
    EmitVertex();
    gl_Position = gl_Position + modelViewProjectionMatrix * vec4(0.1*scale*dx[0]
                                                   ,0.0);
    f_color = vec4(1.0,1.0,1.0,1.0);
    EmitVertex();
    EndPrimitive();
}
