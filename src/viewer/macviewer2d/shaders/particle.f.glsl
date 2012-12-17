
in vec2 st;
void main(void)
{
    float dist=length(st);
    if(dist>0.5) discard;
    gl_FragColor = vec4(0.0,0.0,1.0,1.0);
}
