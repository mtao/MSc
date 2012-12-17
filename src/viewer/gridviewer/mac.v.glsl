#version 330

in int index;
uniform ivec3 dim;
uniform float isolevel;
uniform sampler3D dataFieldTex;

out float g_phi;

void main()
{
    ivec3 coord = ivec3(
                (index)%(dim[0]),
            int(float(index)/dim[0])%dim[1],
            int(float(index)/(dim[0]*dim[1])));
    g_phi = texelFetch(dataFieldTex,coord,0).a-isolevel;
    gl_Position = vec4((coord+vec3(0.5))/dim,1.0);
}
