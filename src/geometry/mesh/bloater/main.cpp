#include "geometry/mesh/io.h"
#include "geometry/mesh/processing.h"
#include <iostream>
#include <fstream>

//Bloat the mesh slightly by moving itself r in the direction of its mesh.normals
//TODO: make this level set method
    template <typename NT>
void expand(Mesh<NT> & mesh,float r)
{
    for(unsigned int i=0; i<mesh.vertices.size(); ++i)
    {
        //printVert(std::cout,mesh.vertices[i]);
        //std::cout << " ";
        //printVert(std::cout,mesh.normals[i]);
        //std::cout << std::endl;
        mesh.vertices[i] += r * mesh.normals[i];
    }
}

/*
float* read(std::ifstream & in, int lines)
{
    
    if(!in.is_open()) return;

    std::istream_iterator<float> eos;
    std::istream_iterator<float> isit(in);
    iterator it=begin();
    for(;isit!=eos; ++it, ++isit)
        *it=*isit;

}
*/
int main(int argc, char * argv[])
{
    if(argc!=4)
    {
        std::cout << "Bad input" << std::endl;
        return -1;
    }
    std::ifstream infile(argv[1]);
    std::ofstream outfile(argv[2]);
    MeshIOUtils::OBJParser<NumericalTraitsXf> x;
    x.read(infile);
    std::cout << "blloat time\n";
    MeshUtils::genTertiaryData(*x.mesh);
    expand(*x.mesh,atof(argv[3]));
    std::cout << "blloat time over\n";
    x.write(outfile);
    std::ofstream outfile2("appended.obj");
    x.write(outfile2);



    return 0;
}
