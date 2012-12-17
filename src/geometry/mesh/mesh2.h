#ifndef MESH2_H
#define MESH2_H
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry/util/types.h"
#include "geometry/util/index.h"



template <typename NT>
struct Mesh2
{
    typedef NT NumTraits;
    typedef typename NumTraits::Vector2 Vector2;

    std::vector<Vector2> vertices;
    std::vector<Vector2> normals;
    std::vector<IndexSet<2> > faces;
};

typedef Mesh2<NumericalTraitsXf> Mesh2f;
typedef Mesh2<NumericalTraitsXd> Mesh2d;


#endif
