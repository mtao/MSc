#version 330

uniform mat4 modelViewProjectionMatrix;
in vec3 vertex;
out vec3 f_vertex;
out float f_len;
out vec3 col;

void main( void )
{

    // Calculate the clip-space coordinates
    gl_Position = modelViewProjectionMatrix * vec4(vertex-vec3(.5,.5,0),1.0);
    f_vertex = vertex;
    f_len = length(vertex);
    col = vec3(1.0);
}
