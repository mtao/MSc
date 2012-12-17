#include "geometry/levelset/levelset.h"
#include <math.h>
#include <iostream>
#define DIM 200
float center_phi(uint i, uint j, uint k)
{
    float x=i;x-=DIM/2;
    float y=j;y-=DIM/2;
    float z=k;z-=DIM/2;
    return 
            x*x+y*y-z*z;
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
