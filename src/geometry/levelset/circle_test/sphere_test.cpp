#include "geometry/levelset/levelset.h"
#include <math.h>
#include <iostream>
#define DIM 100 
float center_phi(uint i, uint j)
{
    float x=i;x-=DIM/2;
    float y=j;y-=DIM/2;
    return std::sqrt(
            x*x+y*y);
}

int main()
{
    LevelSet<> phi(DIM,DIM);
        for(int j=0; j<phi.NJ(); ++j)
            for(int i=0; i<phi.NI(); ++i)
            {
                phi(i,j)=center_phi(i,j)-DIM/5.0;
            }
    std::ofstream out("output.ls");
    phi.write(out);



}
