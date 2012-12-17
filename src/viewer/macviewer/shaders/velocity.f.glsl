in vec4 f_color;
void main(void)
{
    gl_FragColor = vec4(1.0,0.0,0.0,1.0);
    gl_FragColor = f_color;
}
