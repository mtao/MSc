#version 330
layout(points) in;
//layout(points, max_vertices=6) out;
layout(triangle_strip, max_vertices=10) out;
uniform mat4 modelViewProjectionMatrix;
//uniform float scale;
uniform float aspectRatio;
uniform float radius;
out vec2 st;

in vec3 g_vertex[];

out vec3 f_vertex;
void main()
{
    f_vertex = g_vertex[0];
    vec4 pos = gl_in[0].gl_Position;
    //pos = vec4(position,0.0,1.0);

    float radius2= radius;
    float radius3 = radius/aspectRatio;
    gl_Position = pos+vec4(-radius3,-radius2,0.0,0.0);st=vec2(-1,-1); EmitVertex();
    gl_Position = pos+ vec4(radius3,-radius2,0.0,0.0) ;st=vec2(1,-1); EmitVertex();
    gl_Position = pos+ vec4(-radius3,radius2,0.0,0.0) ;st=vec2(-1,1); EmitVertex();
    EndPrimitive();
    gl_Position = pos+ vec4(-radius3,radius2,0.0,0.0) ;st=vec2(-1,1); EmitVertex();
    gl_Position = pos+ vec4(radius3,-radius2,0.0,0.0) ;st=vec2(1,-1); EmitVertex();
    gl_Position = pos+ vec4(radius3,radius2,0.0,0.0)  ;st=vec2(1,1); EmitVertex();
    EndPrimitive();

}
/*
#version 330
uniform mat4 modelViewProjectionMatrix;

layout(points) in;
layout(line_strip, max_vertices=2) out;
out vec4 f_color;
in vec3 dx[];

void main(void)
{
    gl_Position = (gl_in[0].gl_Position);
    f_color = vec4(.2,.2,.2,1.0);
    EmitVertex();
    gl_Position = gl_Position + modelViewProjectionMatrix * vec4(0.1*dx[0]
                                                   ,0.0);
    f_color = vec4(1.0,1.0,1.0,1.0);
    EmitVertex();
    EndPrimitive();
}

*/
