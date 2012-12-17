#include "geometry/marchingcubes/marchingcubes.h"
#include "geometry/grid/mac/mac.h"
#include "geometry/mesh/io.h"
#include <math.h>
#include <iostream>

int main()
{
    MarchingCubeGridf phi(1,1,1);
    std::ifstream phifile("cube.phi");
    phi.read(phifile);
    Meshf mesh;
    phi.populateMesh(mesh,0);
    std::cout << mesh.vertices.size() << std::endl;
    std::cout << mesh.edges.size() << std::endl;
    std::cout << mesh.faces.size() << std::endl;
    std::ofstream out("output.obj");
    MeshIOUtils::OBJParser<NumericalTraitsXf> parser(&mesh);
    parser.write(out);



}
