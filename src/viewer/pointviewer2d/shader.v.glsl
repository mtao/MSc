#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec2 vertex;


void main( void )
{

    // Calculate the clip-space coordinates
    gl_Position = projectionMatrix*viewMatrix*modelMatrix*vec4(vertex,0.0,1.0);
}
