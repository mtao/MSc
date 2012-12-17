#version 120
uniform mat4 modelViewProjectionMatrix;

varying vec4 f_color;
void main( void )
{

    // Calculate the clip-space coordinates
//    gl_Position = modelViewProjectionMatrix * gl_Vertex;
    if(gl_Color.x==1.0)//red = cursor
    {
    gl_Position = modelViewProjectionMatrix * gl_Vertex;
    }
    else//green = screen space
    {
    gl_Position = gl_Vertex;
    }
    
    f_color = gl_Color;


}
