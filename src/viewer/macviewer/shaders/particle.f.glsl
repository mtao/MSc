#version 330
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalTransform;
in vec3 f_vertex;
in vec4 pos;
in vec2 st;
uniform float radius;
void main(void)
{
    float dist=length(st);
    if(dist>0.5) discard;
    float z = sqrt(1-pow(length(st),2.0));
    vec3 n = vec3(st,z);
    n = normalize(n);

    //vec4 pixelPos = modelViewMatrix * vec4(f_vertex+ n*radius,1);
    vec4 eyePos = modelViewMatrix * vec4(f_vertex,1)+n.z*radius;
    vec4 clipPos =projectionMatrix*eyePos;
    gl_FragDepth = (gl_DepthRange.diff * clipPos.z / clipPos.w + gl_DepthRange.far + gl_DepthRange.near)/2;

    const vec3 eye = vec3(0,0,-5);
    //float ang = dot(normalize(pixelPos.xyz - eye),normalTransform*vec4(n,0.0));
    float ang = n.z;
    gl_FragColor = ang * vec4(0.0,0.0,1.0,1.0);
//    gl_FragColor = clipPos;
//    gl_FragColor = clipPos;
}
