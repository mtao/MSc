#include "geometry/levelset/levelset.h"
#include <math.h>
#include <iostream>
#define DIM 400 
float center_phi(uint i, uint j, uint k)
{
    float x=i;x-=DIM/2;
    float y=j;y-=DIM/2;
    float z=k;z-=DIM/2;
    x=x/DIM*3;
    y=y/DIM*3;
    z=z/DIM*3;
    float R = .1;
    float r = 0.05;
    return 
        (x*x+9/4*y*y+z*z-1)*
        (x*x+9/4*y*y+z*z-1)*
        (x*x+9/4*y*y+z*z-1)
        -x*x*z*z*z-9/80*y*y*z*z*z;
    return x*x*x*x+z*z*z - y*z*z;
    return 
        20*(
        (x*x+y*y+z*z+R*R-r*r)*
        (x*x+y*y+z*z+R*R-r*r) - R*R * (x*x+y*y)
        );
}

int main()
{
    LevelSet<> phi(DIM,DIM,DIM);
    for(int k=0; k<phi.NK(); ++k)
        for(int j=0; j<phi.NJ(); ++j)
            for(int i=0; i<phi.NI(); ++i)
            {
                phi(i,j,k)=center_phi(i,j,k);
            }
    std::ofstream out("output.ls");
    phi.write(out);



}
