/*
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform float distance;
uniform float scale;
*/
in int f;
//in vec2 vertex;
void main( void )
{

    // Calculate the clip-space coordinates
    //gl_Position = modelViewProjectionMatrix * vec4(vertex-0.5,0.0,1.0);
    //gl_PointSize = scale/distance;

    gl_Position = vec4(vec3(f)/50.0,1.0);



}
