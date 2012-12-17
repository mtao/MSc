#version 330
##extension GL_EXT_gpu_shader4: require

uniform vec4 u_color;
uniform sampler2D tex;
uniform isampler2D tex2;
in vec3 f_vertex;
in float f_len;

layout(location = 0, index = 0) out vec4 fragmentColor;

void main( void )
{
    // Calculate intensity as max of 0 and dot product of the
    // fragmentNormal and the eye position (0,0,1).
    if(gl_FrontFacing)
    {
        discard;
        //fragmentColor = vec4(1.0)*texture(tex,f_vertex.xy).a;
        fragmentColor = texture(tex,f_vertex.xy);

    }

    else
    {
        //fragmentColor = vec4(1.0) * float(texture(tex2,f_vertex.xy).w);
        /*
        switch (texture(tex2,f_vertex.xy).a)
        {
            case 0:
                fragmentColor=vec4(0.0,1.0,0.0,1.0);
                break;
            case 1:
                fragmentColor=vec4(1.0,0.0,0.0,1.0);
                break;
            case 2:
                fragmentColor=vec4(0.0,0.0,1.0,1.0);
                break;
        }
        */
        int x = int(f_vertex.x * 256);
        int y = int(f_vertex.y * 16);
        fragmentColor = vec4(texture(tex2,f_vertex.xy).w,0.0,0.0,1.0);
        fragmentColor = fragmentColor - vec4(texelFetch(tex2, ivec2(x,y),0).w,0.0,0.0,1.0);
        if (fragmentColor.r < 0)
            fragmentColor.rgb = -fragmentColor.grb;

    }
    //fragmentColor = u_color * f_len;
    //fragmentColor = vec4(vec3(f_vertex.xy,0.0),1.0).xzya;

}
