uniform vec3 dim;

in vec3 vertex;
void main( void )
{

    // Calculate the clip-space coordinates
    gl_Position = vec4(vertex/dim,1.0);
    


}
