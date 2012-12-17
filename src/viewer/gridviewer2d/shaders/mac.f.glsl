#version 330
layout(location = 0, index = 0) out vec4 fragmentColor;
in vec4 f_color;

void main()
{
    fragmentColor = f_color;
}

