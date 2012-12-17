#version 330

uniform vec4 u_color;

layout(location = 0, index = 0) out vec4 fragmentColor;

void main( void )
{
    fragmentColor = vec4(1.0,0.0,0.0,1.0);
}
