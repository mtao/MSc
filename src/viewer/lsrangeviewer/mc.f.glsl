#version 330

layout(location = 0, index = 0) out vec4 fragmentColor;
uniform vec4 u_color;
uniform sampler3D dataFieldTex;
uniform vec3 vertDecals[8];
uniform mat4 modelViewMatrix;
uniform mat3 normalTransform;
uniform bool texture_points;

in vec4 f_color;
in vec3 position;
const vec3 diffuseMaterial = vec3(0.7, 0.7, 0.7);
const vec3 specularMaterial = vec3(0.99, 0.99, 0.99);
const vec3 ambientMaterial = vec3(0.1, 0.1, 0.1);
uniform float isolevel;
uniform float isoAdd;

float pdata(int i)
{
    return (isoAdd - abs(texture(dataFieldTex,position+vertDecals[i]).a-isolevel));
}
float mdata(int i)
{
    return (isoAdd - abs(texture(dataFieldTex,position-vertDecals[i]).a-isolevel));
}

void main( void )
{
    vec3 color=u_color.xyz;





    vec3 grad = vec3( 
            pdata(1)-mdata(1),
            pdata(3)-mdata(3),
            pdata(4)-mdata(4)
            );


    vec3 normalVec = normalize(grad);//normalize(modelMatrix_invTranspose*grad);
    vec4 pos = (modelViewMatrix* vec4(position-vec3(0.5),1.0));
    vec3 eyeDir = pos.xyz/pos.w;//world
    vec3 lightDir=normalize(eyeDir-vec3(0.0,0.0,2.0));


    vec3 ambient = ambientMaterial*u_color.xyz;

    vec3 worldNormal = normalize(normalTransform * normalVec);
    // calculate diffuse component
    float lightAng = max(0.0,dot(worldNormal, lightDir));
    if(lightAng == 0.0)
        {
            discard;
            fragmentColor = vec4(ambient,1.0);
            return;
        }
    vec3 diffuse = lightAng * u_color.rgb;

    // calculate specular component
    vec3 reflection = normalize(reflect(lightDir, worldNormal));
    float spec = max(0.0,dot(-normalize(eyeDir), reflection));
    vec3 specular = pow(spec, 32.0) * specularMaterial;



    fragmentColor = vec4(ambient+diffuse+specular,1.0);


}
