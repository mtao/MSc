#include "geometry/mesh/io.h"
#include <iostream>
#include <fstream>

int main(int argc, char * argv[])
{
    if(argc!=3)
    {
        std::cout << "Bad input" << std::endl;
        return -1;
    }
    std::ifstream infile(argv[1]);
    std::ofstream outfile(argv[2]);
    Mesh<NumericalTraitsXf> mesh;
    MeshIOUtils::OBJParser<NumericalTraitsXf> objparser(&mesh);
    objparser.read(infile);
    MeshIOUtils::writePBRT(mesh,outfile);



    return 0;
}
