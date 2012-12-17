#version 330

in int index;
uniform ivec3 dim;
uniform float isolevel;
uniform sampler3D u;
uniform sampler3D v;
uniform sampler3D w;


out vec3 dx;
void main()
{
    vec3 coord = vec3(
                (index)%(dim[0]),
            int(float(index)/dim[0])%dim[1],
            int(float(index)/(dim[0]*(dim[1]))));
    vec3 pos = coord / dim;
    dx =
            vec3(
                texture(u,pos).a,
                texture(v,pos).a,
                texture(w,pos).a
            );
    gl_Position = vec4(pos + vec3(.5)/dim - vec3(0.5),1.0);
}
