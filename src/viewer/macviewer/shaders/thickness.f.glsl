#version 330
in vec2 st;

void main(void)
{
    float dist=length(st);
    if(dist>0.5) discard;
    float z = sqrt(1-pow(length(st),2.0));
    vec3 n = vec3(st,z);
    n = normalize(n);

    float sigma = 3.0;
    float mu = 0.0;
    float g_dist =0.1*exp(-(dist-mu)*(dist-mu)/(2*sigma));
    gl_FragColor = vec4(g_dist);
}
