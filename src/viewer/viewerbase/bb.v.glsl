#version 330

uniform mat4 modelViewProjectionMatrix;

in vec3 vertex;

void main( void )
{
    gl_Position = modelViewProjectionMatrix * vec4(vertex,1.0);
}
