#version 130

uniform mat4 modelViewMatrix;
varying vec3 fragmentNormal;
varying vec4 pos;
uniform vec4 u_color;

const vec3 diffuseMaterial = vec3(0.7, 0.7, 0.7);
const vec3 specularMaterial = vec3(0.99, 0.99, 0.99);
const vec3 ambientMaterial = vec3(0.1, 0.1, 0.1);

void main( void )
{
    gl_FragColor = vec4(1.0);
    return;
    // Calculate intensity as max of 0 and dot product of the
    // fragmentNormal and the eye position (0,0,1).
    float intensity;

    vec3 eyeDir = pos.xyz/pos.w;//world
    vec3 lightDir=normalize(eyeDir-vec3(0.0,0.0,2.0));


    vec3 ambient = ambientMaterial*u_color.xyz;

    vec3 worldNormal = -normalize(fragmentNormal);
    // calculate diffuse component
    float lightAng = max(0.0,dot(worldNormal, lightDir));
    if(lightAng == 0.0)
        {
            gl_FragColor = vec4(ambient,1.0);
            return;
        }
    vec3 diffuse = lightAng * u_color.rgb;

    // calculate specular component
    vec3 reflection = normalize(reflect(lightDir, worldNormal));
    float spec = max(0.0,dot(normalize(eyeDir), reflection));
    vec3 specular = pow(spec, 32.0) * specularMaterial;



    gl_FragColor = vec4(ambient+diffuse+specular,1.0);

}
