#include "geometry/levelset/levelset.h"
#include <math.h>
#include <iostream>
#define DIM 10

int main()
{
    LevelSet<> phi(DIM,DIM,DIM,Eigen::Vector3f(0.2,0.3,0.4));
    for(int k=0; k<phi.NK(); ++k)
        for(int j=0; j<phi.NJ(); ++j)
            for(int i=0; i<phi.NI(); ++i)
            {
                std::cout << (Eigen::RowVector3f)phi.vertex(i,j,k) << std::endl;
            }
    




}
