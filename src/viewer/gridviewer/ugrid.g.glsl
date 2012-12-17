#version 330

layout(points) in;
//layout(triangle_strip, max_vertices=12) out;
layout(line_strip, max_vertices=12) out;

uniform sampler3D dataFieldTex;
uniform vec4 u_color;
uniform float scale;
uniform vec3 vertDecals[8];
uniform mat4 modelViewProjectionMatrix;
uniform int discard_overflow;
uniform ivec3 dim;

uniform uint shader_type;

in float g_phi[];

out vec4 f_color;

void main()
{
    
    float phi = g_phi[0]/scale*.1;
    if(discard_overflow == 1 && abs(phi)>1.0) return;

    f_color = vec4(0.0);
    gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position
                -vec4(0.5     ,0.5   + 0.5/dim[1]  ,0.5  + 0.5/dim[2]   ,0.0));
    EmitVertex();
    f_color = vec4(max(0.0,10*phi),-min(0.0,10*phi),0.0,1.0);
    gl_Position = modelViewProjectionMatrix*(gl_in[0].gl_Position
                -vec4(0.5+phi     ,0.5   + 0.5/dim[1]  ,0.5  + 0.5/dim[2]   ,0.0));
    EmitVertex();
    EndPrimitive();

}
