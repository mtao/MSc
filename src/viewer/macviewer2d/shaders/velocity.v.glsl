#version 330
in int index;
in float phi;
uniform ivec2 dim;
void main(void)
{
    ivec2 coord = ivec2(index%dim[0], int(index/dim[0]));
    gl_Position = vec4((coord+vec2(.5))/dim,0.0,1.0);
}
