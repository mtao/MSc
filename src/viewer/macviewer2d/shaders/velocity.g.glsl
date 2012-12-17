#version 330
uniform mat4 modelViewProjectionMatrix;

layout(points) in;
layout(line_strip, max_vertices=2) out;
uniform float scale;
uniform sampler2D u;
uniform sampler2D v;
out vec4 f_color;

void main(void)
{
    vec2 dx = 0.1*scale *
            vec2(
                texture(u,gl_in[0].gl_Position.xy).a
            ,
                texture(v,gl_in[0].gl_Position.xy).a
            );
    gl_Position = modelViewProjectionMatrix * (gl_in[0].gl_Position - vec4(0.5,0.5,0,0));
    f_color = vec4(.2);
    EmitVertex();
    gl_Position = gl_Position + modelViewProjectionMatrix * vec4(dx
                                                   ,0.0,0.0);
    f_color = vec4(1.0,0.0,0.0,1.0);
    EmitVertex();
    EndPrimitive();
}
