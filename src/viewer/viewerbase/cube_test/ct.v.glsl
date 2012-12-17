#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

in vec4 vertex;
in vec3 normal;

out vec3 fragmentNormal;

void main( void )
{
    // Transform the normal vector
    fragmentNormal = ( viewMatrix * modelMatrix * vec4( normal, 0.0 ) ).xyz;

    // Calculate the clip-space coordinates
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertex;
}
