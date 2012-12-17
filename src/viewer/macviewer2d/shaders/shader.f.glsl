#version 330
/*
uniform vec4 u_color;
in vec2 st;


layout(location = 0, index = 0) out vec4 fragmentColor;

void main(void)
{



    float dist=length(st);
    if(dist>0.5) discard;
    fragmentColor = vec4(0.0,0.0,1.0,0.5);
    //float dist=length(gl_PointCoord.xy-vec2(0.5,0.5));
    float sigma = 0.2f;
    float coeff = 0.02f * exp(-(dist*dist)/(2*sigma));
    fragmentColor = coeff * fragmentColor ;



}
*/

layout (location =0, index=0) out vec4 fragmentColor;
void main(void)
{
    fragmentColor = vec4(1.0);
}
