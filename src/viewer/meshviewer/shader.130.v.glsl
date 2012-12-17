#version 130

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 modelMatrix_invTranspose;

attribute vec3 vertex;
attribute vec3 normal;

varying vec4 pos;
varying vec3 fragmentNormal;

void main( void )
{
    // Transform the normal vector
    fragmentNormal = ( modelViewMatrix * vec4( normal, 0.0 ) ).xyz;

    // Calculate the clip-space coordinates
    gl_Position = modelViewProjectionMatrix * vec4(vertex,1.0);
    pos = gl_Position;
}
