#version 330

in vec3 fragmentNormal;
uniform vec4 u_color;

layout(location = 0, index = 0) out vec4 fragmentColor;

const vec4 wire_col = vec4(1.0,1.0,1.0,1.0);
noperspective in vec4 dist;

void main( void )
{

    float intensity;
    float d = min(dist[0], min(dist[1],dist[2]));
    float I = exp2(-2*d*d);
    if(I<0.2)
        discard;
    fragmentColor = I * u_color;
}
