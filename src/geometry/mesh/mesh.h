#ifndef MESH_H
#define MESH_H
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry/util/types.h"
#include "geometry/util/index.h"


#ifdef DONT_USE_TRAITS

class Mesh
{
    public:
        void render(bool wireframe, bool normals) const;
        const std::string & fileName(){return filename;}
        int numVerts() {return verts.size();}
        int numEdges() {return edges.size();}
        int numFaces() {return faces.size();}
        std::vector<Vertex> & getVerts() {return verts;}
        std::vector<Vertex> & getNormals() {return normals;}
        std::vector<Edge> & getEdges() {return edges;}
        std::vector<Face> & getFaces() {return faces;}
        void expand(float r);
        void normalize();
        void append(Mesh & m);
        void copy(Mesh & other);
        void writePBRT(std::ofstream & outfile);
        void genTertiaryData();

    protected:
        void genNormals();
        void genEdges();
        std::vector<Vertex> verts;
        std::vector<Vertex> normals;
        std::vector<int> arity;
        std::vector<Face> faces;
        std::vector<Edge> edges;
        std::string filename;
};

#else

template <typename NT>
struct Mesh
{
    typedef NT NumTraits;
    typedef typename NumTraits::Vector3 Vector3;

    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<IndexSet<2> > edges;
    std::vector<IndexSet<3> > faces;
};

typedef Mesh<NumericalTraitsXf> Meshf;
typedef Mesh<NumericalTraitsXd> Meshd;

#endif

#endif
