#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat3 modelMatrix_invTranspose;
uniform vec3 dim;

in vec3 vertex;
in vec3 normal;
in float phi;

out float g_phi;
out VertexData
{
    float phi;
} vertexData;
void main( void )
{

    // Calculate the clip-space coordinates
    gl_Position = vec4(vertex/dim,1.0);
    g_phi=phi;
    vertexData.phi = phi;
    


}
