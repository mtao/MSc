#version 330
layout(location = 0, index = 0) out vec4 fragmentColor;
in vec4 f_color;

void main()
{
    fragmentColor = f_color;
    /*
    if(gl_FrontFacing)
    fragmentColor = vec4(1.0,0.0,0.0,1.0);
    else
        fragmentColor = vec4(0.0,1.0,0.0,1.0);
        */
}

