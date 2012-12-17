#version 330

in int index;
in float phi;
uniform ivec2 dim;
uniform float isolevel;
uniform sampler2D dataFieldTex;
uniform int texture_points;

out float g_phi;

void main()
{
    ivec2 coord = ivec2(index%dim[0], int(index/dim[0]));
    if(texture_points == 1)
        g_phi = texelFetch(dataFieldTex,coord,0).a - isolevel;
    else
        g_phi = phi - isolevel;
    gl_Position = vec4((coord+vec2(.5))/dim,0.0,1.0);
}
