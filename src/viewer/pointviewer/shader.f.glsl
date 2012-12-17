#version 330

uniform vec4 u_color;

layout(location = 0, index = 0) out vec4 fragmentColor;

void main( void )
{
    // Calculate intensity as max of 0 and dot product of the
    // fragmentNormal and the eye position (0,0,1).
    fragmentColor = u_color;
}
