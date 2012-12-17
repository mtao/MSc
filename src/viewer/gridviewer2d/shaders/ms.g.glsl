#version 330

layout(points) in;
layout(line_strip, max_vertices=32) out;

uniform float isolevel;
uniform vec4 u_color;
uniform float scale;
uniform sampler2D dataFieldTex;
uniform isampler2D edgeTableTex;
uniform vec2 vertDecals[4];

uniform mat4 modelViewProjectionMatrix;
uniform bool discard_overflow;
uniform ivec2 dim;


out vec4 f_color;
vec2 squarePos(int i){ 
    return gl_in[0].gl_Position.xy + vertDecals[i]; 
} 

int edgeTableValue(int i, int j){ 
    return texelFetch(edgeTableTex, ivec2(j, i), 0).a; 
} 


vec2 vertexInterp(float isolevel, vec2 v0, float l0, vec2 v1, float l1)
{ 
    return mix(v0, v1, (isolevel-l0)/(l1-l0)); 
}



void main()
{

    vec2 squarePos0 = squarePos(0);
    vec2 squarePos1 = squarePos(1);
    vec2 squarePos2 = squarePos(2);
    vec2 squarePos3 = squarePos(3);
    float squareVal0 =10*texture(dataFieldTex,  squarePos0).a;
    float squareVal1 =10*texture(dataFieldTex,  squarePos1).a;
    float squareVal2 =10*texture(dataFieldTex,  squarePos2).a;
    float squareVal3 =10*texture(dataFieldTex,  squarePos3).a;
    int squareindex = int(squareVal0 < isolevel); 
    squareindex += int(squareVal1 < isolevel)*2; 
    squareindex += int(squareVal2 < isolevel)*4; 
    squareindex += int(squareVal3 < isolevel)*8; 

    

    if (squareindex ==0 || squareindex == 15) 
        return; 

    vec2 vertlist[4]; 

    //Find the vertices where the surface intersects the square 
    vertlist[0] = vertexInterp(isolevel, squarePos0, squareVal0, squarePos1, squareVal1); 
    vertlist[1] = vertexInterp(isolevel, squarePos1, squareVal1, squarePos2, squareVal2); 
    vertlist[2] = vertexInterp(isolevel, squarePos2, squareVal2, squarePos3, squareVal3); 
    vertlist[3] = vertexInterp(isolevel, squarePos3, squareVal3, squarePos0, squareVal0); 

    int i=0;
    f_color = vec4(1.0, 1.0, 1.0, 1.0);

    vec2 position;

    while(true)
    { 
        if(edgeTableValue(squareindex, i)!=-1)
        { 
            //Generate first vertex of triangle// 
            //Fill position varying attribute for fragment shader 
            position = vertlist[edgeTableValue(squareindex, i)];
            gl_Position = modelViewProjectionMatrix
                    * vec4(position-vec2(0.5),0.1,1.0);
            //f_color = texture(dataFieldTex,position+vec2(0.5));
            EmitVertex();
            position = vertlist[edgeTableValue(squareindex, i+1)];
            gl_Position = modelViewProjectionMatrix
                    * vec4(position-vec2(0.5),0.1,1.0);
            //f_color = texture(dataFieldTex,position+vec2(0.5));
            EmitVertex();
            EndPrimitive();
        }
        else
        {
            return;
        }

        i=i+2;


    }



}
