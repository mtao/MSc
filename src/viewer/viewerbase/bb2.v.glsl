#version 330

uniform mat4 modelViewProjectionMatrix;

in vec2 vertex;

void main( void )
{
    gl_Position = modelViewProjectionMatrix * vec4(vertex,0.0,1.0);
}


