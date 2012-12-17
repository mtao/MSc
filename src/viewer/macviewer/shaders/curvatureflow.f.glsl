#version 330
uniform sampler2D thickness;
uniform sampler2D depth;
uniform ivec2 V;
in vec2 st;
const vec3 diffuseMaterial = vec3(0.1, 0.1, 0.1);
const vec3 specularMaterial = vec3(0.49, 0.49, 0.49);
const vec3 ambientMaterial = vec3(0.1, 0.1, 0.1);
const vec4 u_color = vec4(0.0,0.0,1.0,1.0);
uniform mat4 invPersMatrix;
uniform mat4 invViewMatrix;

float depthToZ(vec2 pos)
{
    return (2.0 * texture(depth,pos).r - gl_DepthRange.near - gl_DepthRange.far) /
        (gl_DepthRange.far - gl_DepthRange.near);
}
layout(location = 0, index = 0) out vec4 fragmentColor;
void main(void)
{

    float thickval = 0;
    if(
            texture(thickness,st).b > 0)
    {
            thickval = 1-.5*(
            +texture(thickness,st+vec2(-1,-1)/V).b
            +texture(thickness,st+vec2(-1, 0)/V).b
            +texture(thickness,st+vec2(-1, 1)/V).b
            +texture(thickness,st+vec2(0,-1)/V).b
            +texture(thickness,st+vec2(0, 0)/V).b
            +texture(thickness,st+vec2(0, 1)/V).b
            +texture(thickness,st+vec2(1,-1)/V).b
            +texture(thickness,st+vec2(1, 0)/V).b
            +texture(thickness,st+vec2(1, 1)/V).b
            );
            thickval =.5 * exp(thickval);
    }
    gl_FragDepth = texture(depth,st).r;
    vec4 ndcPos;
    ndcPos.xy = ((2.0 * gl_FragCoord.xy) ) / (V) - 1;
    ndcPos.z = depthToZ(st);
    ndcPos.w = 1.0;
    vec4 pos = invViewMatrix * invPersMatrix * ndcPos;


    vec2 C = 2.0/(V);//*(st));
    vec3 fragmentNormal = vec3(-C.y,-C.x,C.x*C.y);
    fragmentNormal.x = .5*
            (
            depthToZ(st+vec2(1,0)/V)
            -depthToZ(st+vec2(-1,0)/V)
                );
    fragmentNormal.y = .5*
            (
            depthToZ(st+vec2(0,1)/V)
            -depthToZ(st+vec2(0,-1)/V)
                );
    if(abs(fragmentNormal.x) > 1 || abs(fragmentNormal.y)>1)
        discard;
    fragmentNormal.z = (1-ndcPos.z);
    fragmentNormal = normalize(fragmentNormal
                                  *vec3(-2*C.yx,C.x*C.y)
                               );
    //fragmentNormal.xy = normalize(fragmentNormal.xy);
    fragmentColor = vec4(fragmentNormal,1.0);
    //fragmentColor = vec4(vec3(fragmentNormal.z),1.0);









    //PHONG MODEL
    float intensity;

    vec3 eyeDir = pos.xyz/pos.w;//TODO: calculate pos
    vec3 lightDir=normalize(eyeDir-vec3(0.0,0.0,2.0));


    vec3 ambient = ambientMaterial*u_color.xyz;

    vec3 worldNormal = -normalize(fragmentNormal);
    if(!gl_FrontFacing)
    {
        worldNormal = -worldNormal;
    }
    // calculate diffuse component
    float lightAng = max(0.0,dot(worldNormal, lightDir));
    vec3 diffuse = lightAng * u_color.rgb;

    // calculate specular component
    vec3 reflection = normalize(reflect(lightDir, worldNormal));
    float spec = max(0.0,dot(normalize(eyeDir), reflection));
    vec3 specular = pow(spec, 10.0) * specularMaterial;

    if(lightAng == 0.0)
    {
        diffuse = vec3(0);
        specular = vec3(0);
    }





    fragmentColor = vec4(0.0,0.0,thickval, 1.2-.5*thickval);
    fragmentColor.xyz = fragmentColor.xyz + specular;
}
