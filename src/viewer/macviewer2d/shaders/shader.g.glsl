#version 330
/*
layout(points) in;
layout(triangle_strip, max_vertices=6) out;
uniform float scale;
uniform float distance;
uniform float aspectRatio;
out vec2 st;

void main()
{
    gl_PointSize = gl_in[0].gl_PointSize;
    vec4 pos = gl_in[0].gl_Position;

    float factor = 0.1/(distance*scale);
    float factor2 = factor * aspectRatio;
    gl_Position = pos+vec4(-factor,-factor2,0.0,0.0);st=vec2(-0.5,-0.5); EmitVertex();
    gl_Position = pos+vec4(factor,-factor2,0.0,0.0) ;st=vec2(0.5,-0.5); EmitVertex();
    gl_Position = pos+vec4(-factor,factor2,0.0,0.0) ;st=vec2(-0.5,0.5); EmitVertex();
    EndPrimitive();
    gl_Position = pos+vec4(-factor,factor2,0.0,0.0) ;st=vec2(-0.5,0.5); EmitVertex();
    gl_Position = pos+vec4(factor,-factor2,0.0,0.0) ;st=vec2(0.5,-0.5); EmitVertex();
    gl_Position = pos+vec4(factor,factor2,0.0,0.0)  ;st=vec2(0.5,0.5); EmitVertex();
    EndPrimitive();

}
*/
layout(points) in;
layout(points, max_vertices=1) out;

void main()
{
    vec4 pos = gl_in[0].gl_Position;
    gl_Position = pos;
    EmitVertex();

}
