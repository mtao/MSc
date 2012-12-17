#version 330
in vec2 st;
uniform sampler2D depth;
uniform ivec2 V;
float radius;
float depthToZ(vec2 pos)
{
    return (2.0 * texture(depth,pos).r - gl_DepthRange.near - gl_DepthRange.far) /
            (gl_DepthRange.far - gl_DepthRange.near);
}

float dzdx(vec2 pos)
{
    return .5*
            (
                depthToZ(st+vec2(1,0)/V)
                -depthToZ(st+vec2(-1,0)/V)
                );
}
float dzdy(vec2 pos)
{
    return .5*
            (
                depthToZ(st+vec2(0,1)/V)
                -depthToZ(st+vec2(0,-1)/V)
                );
}

layout(location = 0, index = 0) out vec4 fragmentColor;
void main(void)
{
    gl_FragDepth = texture(depth,st).r;
    if(gl_FragDepth == 1) return;
    vec2 d = vec2(dzdx(st),dzdy(st));
    vec2 d2 = vec2(
                dzdx(st+vec2(1,0)/V)
                -dzdx(st+vec2(-1,0)/V)
                ,

                dzdy(st+vec2(0,1)/V)
                -dzdy(st+vec2(0,-1)/V)
                );
    if(abs(length(d2)) > 0.0)
    {
        if(d.x < 2*radius)
        {
            gl_FragDepth = gl_FragDepth - .2*d.x;
        }
        if(d.y < 2*radius)
        {
            gl_FragDepth = gl_FragDepth - .2*d.y;
        }
    }
    gl_FragDepth = (gl_DepthRange.diff * gl_FragDepth + gl_DepthRange.far + gl_DepthRange.near)/2;
    return;
    /*
    float fd = depthToZ(st);
    vec2 C = 2.0/(V);
    vec2 d = vec2(dzdx(st),dzdy(st));
    vec2 d2 = vec2(
                dzdx(st+vec2(1,0)/V)
                -dzdx(st+vec2(-1,0)/V)
                ,

                dzdy(st+vec2(0,1)/V)
                -dzdy(st+vec2(0,-1)/V)
                );
    float D = C.y * C.y * d.x * d.x + C.x * C.x * d.y * d.y + C.x * C.x * C.y * C.y * (1-fd)*(1-fd);

    vec2 dD =vec2(
                 2*d.x * d2.x

                ,
                 2*d.y * d2.y
                );

    vec2 E = vec2(0.5 * d.x * dD.x - d2.x * D
                  ,
                  0.5 * d.y * dD.y - d2.y * D);


    gl_FragDepth = depthToZ(st) + .5*(C.y * E.x + C.x * E.y) / pow(D,1.5);
    fragmentColor = vec4(5*(C.y * E.x + C.x * E.y) ,0,0,1);
    fragmentColor.g = -fragmentColor.r;
    fragmentColor = vec4(abs(d2),0,1);
    gl_FragDepth = fd;
    gl_FragDepth = (gl_DepthRange.diff * gl_FragDepth + gl_DepthRange.far + gl_DepthRange.near)/2;
    gl_FragDepth = texture(depth,st).r;
    */
}
