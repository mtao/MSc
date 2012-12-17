#version 330

layout(points) in;
layout(triangle_strip, max_vertices=12) out;

//uniform sampler2D dataFieldTex;
uniform vec4 u_color;
uniform float scale;
uniform sampler3D dataFieldTex;
uniform mat4 modelViewProjectionMatrix;
uniform bool discard_overflow;
uniform ivec2 dim;

uniform uint shader_type;

in float g_phi[];

out vec4 f_color;

void main()
{
    float phi = g_phi[0]/scale;
    if(discard_overflow && abs(phi)>1.0) return;
    f_color = vec4(max(0.0,phi),-min(0.0,phi),0.0,0.5);

    gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position
            -vec4(0.5 + 0.5/dim[0]    ,0.5 + 0.5/dim[1]    ,0.0    ,0.0));
    EmitVertex();                                                                                  
    gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position
            -vec4(0.5 - 0.5/dim[0]    ,0.5 + 0.5/dim[1]    ,0.0    ,0.0));
    EmitVertex();                                                                                  
    gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position
            -vec4(0.5 + 0.5/dim[0]    ,0.5 - 0.5/dim[1]    ,0.0    ,0.0));
    EmitVertex();                                                                                  
    EndPrimitive();                                                                                 
                                                                                                   
                                                                                                   
    gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position
            -vec4(0.5 + 0.5/dim[0]    ,0.5 - 0.5/dim[1]    ,0.0    ,0.0));
    EmitVertex();                                                                                  
    gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position
            -vec4(0.5 - 0.5/dim[0]    ,0.5 + 0.5/dim[1]    ,0.0    ,0.0));
    EmitVertex();                                                                                  
    gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position
            -vec4(0.5 - 0.5/dim[0]    ,0.5 - 0.5/dim[1]    ,0.0    ,0.0));
    EmitVertex();
    EndPrimitive();
    


}