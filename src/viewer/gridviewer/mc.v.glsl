#version 330

uniform ivec3 dim;

in int index;
void main( void )
{

    // Calculate the clip-space coordinates
    vec3 vertex= vec3(
                (index)%(dim[0]),
            int(float(index)/dim[0])%dim[1],
            int(float(index)/(dim[0]*dim[1])))/dim;
    gl_Position = vec4(vertex,1.0);
    


}
