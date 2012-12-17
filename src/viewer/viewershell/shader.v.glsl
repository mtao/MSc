uniform vec4 modelViewProjectionMatrix;

in vec3 vertex;
void main( void )
{

    // Calculate the clip-space coordinates
    gl_Position = modelViewProjectionMatrix * vec4(vertex,1.0);
    


}
