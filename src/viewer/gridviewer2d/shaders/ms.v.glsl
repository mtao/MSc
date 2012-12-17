#version 330


in int index;
uniform ivec2 dim;
void main()
{
    vec2 vertex = vec2(index%dim[0]+.5, int(index/dim[0])+.5)/dim;
    gl_Position = vec4(vertex,0.0,1.0);
}
