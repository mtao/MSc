#include "geometry/surface_reconstruction/marchingsquares.h"
#include "geometry/mesh/io2.h"
#include <math.h>
#include <iostream>
#define DIM 300
float center_phi(uint i, uint j)
{
    float x=i;x-=DIM/2;
    float y=j;y-=DIM/2;
    return std::sqrt(
            x*x+y*y);
}

int main()
{
    MarchingSquaresGrid<> phi(DIM,DIM);
        for(int j=0; j<phi.NJ(); ++j)
            for(int i=0; i<phi.NI(); ++i)
            {
                phi(i,j)=center_phi(i,j)-DIM/5.0;
            }
    Mesh2<MarchingSquaresGrid<>::NumTraits> * mesh = phi.genMesh(0);
    std::cout << mesh->vertices.size() << std::endl;
    std::cout << mesh->faces.size() << std::endl;
    std::ofstream out("output.obj");
    Mesh2IOUtils::OBJParser<Mesh2<MarchingSquaresGrid<>::NumTraits>::NumTraits> parser(mesh);
    parser.write(out);





}
