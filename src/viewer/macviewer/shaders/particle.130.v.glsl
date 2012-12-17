#version 130
uniform mat4 modelViewProjectionMatrix;

attribute vec3 vertex;
void main( void )
{

    // Calculate the clip-space coordinates
    //gl_Position = modelViewMatrix * vec4(vertex-0.5,0.0,1.0);
    gl_Position = modelViewProjectionMatrix * vec4(vertex-vec3(0.5),1.0);
    //position= (gl_Position.xy/gl_Position.w);
    //gl_Position = projectionMatrix * gl_Position;


}
