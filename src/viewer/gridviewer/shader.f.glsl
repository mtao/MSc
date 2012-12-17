#version 330

uniform vec4 u_color;
uniform sampler3D dataFieldTex;
uniform vec3 vertDecals[8];
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat3 modelMatrix_invTranspose;
uniform bool texture_points;

layout(location = 0, index = 0) out vec4 fragmentColor;
in vec3 position;

const vec3 diffuseMaterial = vec3(0.7, 0.7, 0.7);
const vec3 specularMaterial = vec3(0.99, 0.99, 0.99);
const vec3 ambientMaterial = vec3(0.1, 0.1, 0.1);

void main(void)
{



    vec3 grad = vec3( 
            texture(dataFieldTex,position+vertDecals[1]).a-texture(dataFieldTex,position-vertDecals[1]).a,
            texture(dataFieldTex,position+vertDecals[3]).a-texture(dataFieldTex,position-vertDecals[3]).a,
            texture(dataFieldTex,position+vertDecals[4]).a-texture(dataFieldTex,position-vertDecals[4]).a
            );


    vec3 normalVec = normalize(grad);//normalize(modelMatrix_invTranspose*grad);
    vec4 pos = (viewMatrix * modelMatrix * vec4(position-vec3(0.5),1.0));
    vec3 eyeDir = pos.xyz/pos.w;//world
    vec3 lightDir=normalize(vec3(5.0,3.0,0.0)-eyeDir);


    vec3 ambient = ambientMaterial*u_color.rgb;

    vec3 worldNormal = normalize((viewMatrix*modelMatrix*vec4(normalVec,0.0)).xyz);
    // calculate diffuse component
    float lightAng = max(0.0,dot(worldNormal, lightDir));
    vec3 diffuse = lightAng * u_color.rgb;

    // calculate specular component
    vec3 reflection = normalize(reflect(lightDir, worldNormal));
    float spec = max(0.0,dot(normalize(eyeDir), reflection));
    vec3 specular = pow(spec, 32.0) * specularMaterial;



    if(texture_points)
    {

    fragmentColor = vec4(normalVec,1.0);
    } else
    if(false && lightAng < 0)
    {
        fragmentColor = vec4(ambient,1.0);
    }
    else
    {
    fragmentColor = vec4(ambient+diffuse+specular,1.0);
    }


}
