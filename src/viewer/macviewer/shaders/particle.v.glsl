#version 330
uniform mat4 modelViewProjectionMatrix;

uniform sampler3D u;
uniform sampler3D v;
uniform sampler3D w;
in vec3 vertex;
out vec3 g_vertex;
out vec3 dx;
void main( void )
{

    // Calculate the clip-space coordinates
    //gl_Position = modelViewMatrix * vec4(vertex-0.5,0.0,1.0);
    gl_Position = modelViewProjectionMatrix * vec4(vertex-vec3(0.5),1.0);
    g_vertex = vertex-vec3(0.5);
    dx =
            vec3(
                texture(u,vertex).a,
                texture(v,vertex).a,
                texture(w,vertex).a
            );
    //position= (gl_Position.xy/gl_Position.w);
    //gl_Position = projectionMatrix * gl_Position;


}
