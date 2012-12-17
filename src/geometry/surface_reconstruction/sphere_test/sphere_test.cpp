#include "geometry/marchingcubes/marchingcubes.h"
#include "geometry/mesh/io.h"
#include <math.h>
#include <iostream>
#define DIM 300
float center_phi(uint i, uint j, uint k)
{
    float x=i;x-=DIM/2;
    float y=j;y-=DIM/2;
    float z=k;z-=DIM/2;
    return std::sqrt(
            x*x+y*y+z*z);
}

int main()
{
    MarchingCubeGrid<> phi(DIM,DIM,DIM);
    for(int k=0; k<phi.NK(); ++k)
        for(int j=0; j<phi.NJ(); ++j)
            for(int i=0; i<phi.NI(); ++i)
            {
                phi(i,j,k)=center_phi(i,j,k)-DIM/5.0;
            }
    Mesh<MarchingCubeGrid<>::NumTraits> * mesh = phi.genMesh(0);
    std::cout << mesh->vertices.size() << std::endl;
    std::cout << mesh->edges.size() << std::endl;
    std::cout << mesh->faces.size() << std::endl;
    std::ofstream out("output.obj");
    MeshIOUtils::OBJParser<Mesh<MarchingCubeGrid<>::NumTraits>::NumTraits> parser(mesh);
    parser.write(out);





}
