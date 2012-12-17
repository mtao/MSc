#version 330
layout(points) in;
//layout(points, max_vertices=6) out;
layout(triangle_strip, max_vertices=10) out;
uniform mat4 modelViewProjectionMatrix;
//uniform float scale;
uniform float aspectRatio;
uniform float radius;
out vec2 st;

in vec2 position[];
void main()
{
    vec4 pos = gl_in[0].gl_Position;
    //pos = vec4(position,0.0,1.0);

    float radius2= radius*aspectRatio;
    gl_Position = pos+modelViewProjectionMatrix * vec4(-radius,-radius,0.0,0.0);st=vec2(-1,-1); EmitVertex();
    gl_Position = pos+modelViewProjectionMatrix * vec4(radius,-radius,0.0,0.0) ;st=vec2(1,-1); EmitVertex();
    gl_Position = pos+modelViewProjectionMatrix * vec4(-radius,radius,0.0,0.0) ;st=vec2(-1,1); EmitVertex();
    EndPrimitive();
    gl_Position = pos+modelViewProjectionMatrix * vec4(-radius,radius,0.0,0.0) ;st=vec2(-1,1); EmitVertex();
    gl_Position = pos+modelViewProjectionMatrix * vec4(radius,-radius,0.0,0.0) ;st=vec2(1,-1); EmitVertex();
    gl_Position = pos+modelViewProjectionMatrix * vec4(radius,radius,0.0,0.0)  ;st=vec2(1,1); EmitVertex();
    EndPrimitive();

}
