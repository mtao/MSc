#version 330
layout(points) in;
layout(triangle_strip, max_vertices=12) out;


uniform sampler3D dataFieldTex;
uniform isampler2D edgeTableTex; 
uniform isampler2D triTableTex; 
uniform vec3 vertDecals[8]; 
uniform float isolevel;
uniform float isoAdd;




uniform mat4 modelViewProjectionMatrix;



uniform bool texture_points;
uniform float scale;
in float g_phi[];
out vec4 f_color;
out vec3 position;













vec3 cubePos(int i){ 
    return gl_in[0].gl_Position.xyz + vertDecals[i]; 
} 

int triTableValue(int i, int j){ 
    return texelFetch(triTableTex, ivec2(j, i), 0).a; 
} 


vec3 vertexInterp(float isolevel, vec3 v0, float l0, vec3 v1, float l1)
{ 
    return mix(v0, v1, (isolevel-l0)/(l1-l0)); 
}





void main(void)
{
    vec3 cubePos0 = cubePos(0);
    vec3 cubePos1 = cubePos(1);
    vec3 cubePos2 = cubePos(2);
    vec3 cubePos3 = cubePos(3);
    vec3 cubePos4 = cubePos(4);
    vec3 cubePos5 = cubePos(5);
    vec3 cubePos6 = cubePos(6);
    vec3 cubePos7 = cubePos(7);
    float cubeVal0 =(isoAdd - abs(texture(dataFieldTex,  cubePos0).a-isolevel));
    float cubeVal1 =(isoAdd - abs(texture(dataFieldTex,  cubePos1).a-isolevel));
    float cubeVal2 =(isoAdd - abs(texture(dataFieldTex,  cubePos2).a-isolevel));
    float cubeVal3 =(isoAdd - abs(texture(dataFieldTex,  cubePos3).a-isolevel));
    float cubeVal4 =(isoAdd - abs(texture(dataFieldTex,  cubePos4).a-isolevel));
    float cubeVal5 =(isoAdd - abs(texture(dataFieldTex,  cubePos5).a-isolevel));
    float cubeVal6 =(isoAdd - abs(texture(dataFieldTex,  cubePos6).a-isolevel));
    float cubeVal7 =(isoAdd - abs(texture(dataFieldTex,  cubePos7).a-isolevel));

    int cubeindex = int(cubeVal0 < 0); 
    cubeindex += int(cubeVal1 < 0)*2; 
    cubeindex += int(cubeVal2 < 0)*4; 
    cubeindex += int(cubeVal3 < 0)*8; 
    cubeindex += int(cubeVal4 < 0)*16; 
    cubeindex += int(cubeVal5 < 0)*32; 
    cubeindex += int(cubeVal6 < 0)*64; 
    cubeindex += int(cubeVal7 < 0)*128;

    //Cube is entirely in/out of the surface 
    if (cubeindex ==0 || cubeindex == 255) 
        return; 

    vec3 vertlist[12]; 

    //Find the vertices where the surface intersects the cube 
    vertlist[0] = vertexInterp(0, cubePos0, cubeVal0, cubePos1, cubeVal1); 
    vertlist[1] = vertexInterp(0, cubePos1, cubeVal1, cubePos2, cubeVal2); 
    vertlist[2] = vertexInterp(0, cubePos2, cubeVal2, cubePos3, cubeVal3); 
    vertlist[3] = vertexInterp(0, cubePos3, cubeVal3, cubePos0, cubeVal0); 
    vertlist[4] = vertexInterp(0, cubePos4, cubeVal4, cubePos5, cubeVal5); 
    vertlist[5] = vertexInterp(0, cubePos5, cubeVal5, cubePos6, cubeVal6); 
    vertlist[6] = vertexInterp(0, cubePos6, cubeVal6, cubePos7, cubeVal7); 
    vertlist[7] = vertexInterp(0, cubePos7, cubeVal7, cubePos4, cubeVal4); 
    vertlist[8] = vertexInterp(0, cubePos0, cubeVal0, cubePos4, cubeVal4); 
    vertlist[9] = vertexInterp(0, cubePos1, cubeVal1, cubePos5, cubeVal5); 
    vertlist[10] = vertexInterp(0, cubePos2, cubeVal2, cubePos6, cubeVal6); 
    vertlist[11] = vertexInterp(0, cubePos3, cubeVal3, cubePos7, cubeVal7); 












    int i=0;

    while(true)
    { 
        if(triTableValue(cubeindex, i)!=-1)
        { 
            //Generate first vertex of triangle// 
            //Fill position varying attribute for fragment shader 
            position = vertlist[triTableValue(cubeindex, i)];
            gl_Position = modelViewProjectionMatrix * vec4(position-vec3(0.5),1.0);
            f_color = texture(dataFieldTex,position);
            EmitVertex();
            position = vertlist[triTableValue(cubeindex, i+1)];
            gl_Position = modelViewProjectionMatrix * vec4(position-vec3(0.5),1.0);
            f_color = texture(dataFieldTex,position);
            EmitVertex();
            position = vertlist[triTableValue(cubeindex, i+2)];
            gl_Position = modelViewProjectionMatrix * vec4(position-vec3(0.5),1.0);
            f_color = texture(dataFieldTex,position);
            EmitVertex();
            EndPrimitive();
        }
        else
        {
            return;
        }

        i=i+3;


    }
}


