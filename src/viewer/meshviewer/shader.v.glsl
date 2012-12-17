#version 330

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalTransform;

in vec3 vertex;
in vec3 normal;

out vec4 pos;
out vec3 fragmentNormal;

void main( void )
{
    // Transform the normal vector
    fragmentNormal = ( normalTransform * normal );

    // Calculate the clip-space coordinates
    gl_Position = modelViewProjectionMatrix * vec4(vertex,1.0);
    pos = gl_Position;
}
