#include <QtOpenGL>
#include "geometry/mesh/io.h"
#include "geometry/mesh/processing.h"
#include "geometry/mesh/render.h"
#include <fstream>
#include <iostream>
#include <string>
int main()
{
    Mesh<NumericalTraitsXf> * mesh = new Mesh<NumericalTraitsXf>;
    std::ifstream in("qt.obj");
    MeshIOUtils::OBJParser<NumericalTraitsXf > parser(mesh);
    parser.read(in);
    std::cout << mesh->vertices.size() << std::endl;
    MeshIOUtils::OBJParser<NumericalTraitsXf> parser2;
    parser2.read("qt.obj");
    std::cout << parser2.mesh->vertices.size() << std::endl;


}
