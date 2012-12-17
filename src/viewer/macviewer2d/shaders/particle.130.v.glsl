#version 130
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform float distance;
uniform float scale;
uniform vec3 dim;

attribute vec2 vertex;
varying vec2 position;
void main( void )
{

    // Calculate the clip-space coordinates
    //gl_Position = modelViewMatrix * vec4(vertex-0.5,0.0,1.0);
    gl_Position = modelViewProjectionMatrix * vec4(vertex-vec2(0.5),0.0,1.0);
    //position= (gl_Position.xy/gl_Position.w);
    //gl_Position = projectionMatrix * gl_Position;


}
