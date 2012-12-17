#version 130

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

in vec3 vertex;

void main( void )
{
    gl_Position = modelViewProjectionMatrix * vec4(vertex,1.0);
}
