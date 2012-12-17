#version 330

in vec3 fragmentNormal;
uniform vec4 u_color;

layout(location = 0, index = 0) out vec4 fragmentColor;

void main( void )
{
    // Calculate intensity as max of 0 and dot product of the
    // fragmentNormal and the eye position (0,0,1).
    float intensity;
    intensity = max( abs(dot( fragmentNormal, vec3( 0.0, 0.0, 1.0 ) )), 0.15 );
    fragmentColor = intensity * u_color;
}
