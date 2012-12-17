#version 130

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelViewMatrix;

in vec2 vertex;

void main( void )
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex,0.0,1.0);
}
