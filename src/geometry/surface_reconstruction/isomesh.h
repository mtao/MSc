#ifndef ISOSURFACE_MESH_H
#define ISOSURFACE_MESH_H
#include "edgestorage.h"
#include "geometry/mesh/mesh.h"

#include <vector>

#ifdef DONT_USE_TRAITS
template <typename LSSTORAGE=std::vector<float> >
class IsosurfaceMesh: public Mesh
{
    public:
        inline EdgeStorage<int> & getEdgeData(){return m_edgeData;}
    protected:
        EdgeStorage<int> m_edgeData;
        



};

#else




#endif
#endif
