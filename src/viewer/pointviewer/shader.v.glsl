#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat3 modelMatrix_invTranspose;

in vec3 vertex;
in vec3 normal;


void main( void )
{

    // Calculate the clip-space coordinates
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex,1.0);
}
