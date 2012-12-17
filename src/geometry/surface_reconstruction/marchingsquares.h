#ifndef MARCHING_SQUARES_H
#define MARCHING_SQUARES_H

#include <iostream>
#include "geometry/levelset/levelset.h"
#include "edgestorage2.h"
#include "geometry/mesh/mesh2.h"
#include <algorithm>
#include <vector>




template <typename GT=GridTraitsf >
class MarchingSquaresGrid: public LevelSet<GT>
{
    public:
        typedef IndexSet<2> Index2;
        typedef Grid<GT> GridParent;
        typedef typename GT::Scalar Scalar;
        typedef NumericalTraits<Scalar> NumTraits;
        typedef typename NumTraits::Vector2 Vector2;
        using GridParent::m_NI;
        using GridParent::m_NJ;
        using GridParent::val;
        using GridParent::read;
        using GridParent::write;
        using GridParent::operator();
        MarchingSquaresGrid() {}
        MarchingSquaresGrid(uint ni, uint nj): LevelSet<GT>(ni,nj){}
        MarchingSquaresGrid(const LevelSet<GT> & phi_): LevelSet<GT>(phi_) {}
        void populateMesh(
                Mesh2<NumTraits> & mesh, Scalar level);
        Mesh2<NumTraits> * genMesh(Scalar level)
        {
            Mesh2<NumTraits> * mesh = new Mesh2<NumTraits>();
            populateMesh(*mesh,level);
            return mesh;
        }
        static const int m_edgeTable[16][5];
    private:
        EdgeStorage2<int> m_edgeData;



        inline void shiftPos(Index2 & v, char mask);
        uint edgeIndex(Mesh2<NumTraits> & mesh, uint i, uint j,uint edgeNo, Scalar level);
        uint localEdgeInd(uint i, uint j,uint &edgeNo, Scalar level);
        inline void localEdgeIndMask(uint i, uint j, uint & edgeInd, Scalar level);
        inline void edgeVertMasks(uint edgeNo, char &lmask, char &umask);
};

typedef MarchingSquaresGrid<GridTraitsf> MarchingSquaresGridf;
typedef MarchingSquaresGrid<GridTraitsd> MarchingSquaresGridd;

/*
 * 0 0 :0
 * 0 1 :1
 * 1 1 :2
 * 1 0 :3
 */
//Iterate through every voxel and creat the triangles described in m_triTable
    template <typename GT>
void MarchingSquaresGrid<GT>::populateMesh(
        Mesh2<NumTraits> & mesh, Scalar level)
{
    m_edgeData.resize(m_NI,m_NJ);
    m_edgeData.clear();
    Edge f;
    uint i,j,l;
    uint edgeInd;


    //N?-1 because there are N?-1 cells
        for(j=0; j<m_NJ-1; ++j)
        {
            for(i=0; i<m_NI-1; ++i)
            {
                localEdgeIndMask(i,j,edgeInd,level);

                for(l=0; m_edgeTable[edgeInd][l]!=-1; l+=2)
                {
                    //std::cout << ((*this)(i,j)> level) << " ";
                    //std::cout << ((*this)(i,j+1) > level) << " ";
                    //std::cout << ((*this)(i+1,j+1) > level) << " ";
                    //std::cout << ((*this)(i+1,j) > level) << ": ";
                    //std::cout << m_edgeTable[edgeInd][l]  << " " << 
                 //m_edgeTable[edgeInd][l+1] << std::endl;

                    f[0]=edgeIndex(mesh, i,j,m_edgeTable[edgeInd][l]     ,level);
                    f[1]=edgeIndex(mesh, i,j,m_edgeTable[edgeInd][l+1]   ,level);
                    mesh.faces.push_back(f);
                }


            }

            std::cout << "=" << std::endl;
        }

}




//Edge labeling:
/*
 * 0    : 0 1
 * 1    : 1 2
 * 2    : 2 1
 * 3    : 1 0
 */
//map masks to index triplets
    template <typename GT>
inline void MarchingSquaresGrid<GT>::shiftPos(Index2 & v, char mask)
{
    if(mask & 1) ++v[0];
    if(mask & 2) ++v[1];
}

    template <typename GT>
inline void MarchingSquaresGrid<GT>::localEdgeIndMask(uint i, uint j, uint & edgeInd, Scalar level)
{
    edgeInd=0;
    if((*this)(i  ,j  )<level) edgeInd |= 1;
    if((*this)(i  ,j+1)<level) edgeInd |= 2;
    if((*this)(i+1,j+1)<level) edgeInd |= 4;
    if((*this)(i+1,j  )<level) edgeInd |= 8;
    //if(edgeInd !=0 && edgeInd != 15)
    //std::cout << edgeInd << std::endl;
}

    template <typename GT>
inline void MarchingSquaresGrid<GT>::edgeVertMasks(uint edgeNo, char &lmask, char &umask)
{
    //Identify which vertices are involved w.r.t to i,j,k
    switch(edgeNo)
    {
        case 0:
            //0 0 => 0 1
            umask=2;
            break;
        case 1:
            //0 1 => 1 1
            lmask=2;
            umask=3;
            break;
        case 2:
            //1 1 => 1 0
            lmask=1;
            umask=3;
            break;
        case 3:
            //1 0 => 0 0
            umask=1;
            break;
    }
}

//returns the vert index for the edge edgeNo for voxel i,j,k
//  (it also creates the vertex if it doesn't exist yet)
    template <typename GT>
uint MarchingSquaresGrid<GT>::edgeIndex(
        Mesh2<NumTraits> & mesh, 
        uint i, uint j, uint edgeNo, Scalar level)
{
    std::vector<Vector2> & verts = mesh.vertices;
    //lind = lower index in lexicographical order
    //uind = upper index in lexicographical order
    //dind = uind - lind
    //?mask = more compact notation for ind as a bitmask
    char lmask=0,umask=0,dmask=0;
    Index2 dind={{0}};
    Index2 lind={{i,j}}, uind={{i,j}};

    edgeVertMasks(edgeNo,lmask,umask);

    //edgePtr = pointer to vert index for the vertex on that particular edge
    //  Edges are identified by the lower index and dimension they lie on
    int * edgePtr=0;
    dmask=umask-lmask;
    //std::cout << umask << " " << lmask << " " << dmask << std::endl;
    shiftPos(lind,lmask);
    switch(dmask)
    {
        case 1:
            edgePtr=&m_edgeData.u(lind);
            break;
        case 2:
            edgePtr=&m_edgeData.v(lind);
            break;
    }
    //std::cout << *edgePtr << std::endl;
    //If this edge doesn't have a vertex associated with it yet and the edge is valid
    //  create one
    if((*edgePtr)==-1 && edgeNo!=-1)
    {
        shiftPos(uind,umask);
        shiftPos(dind,dmask);
        //lerp to find where the 0 isosurface must lie
        //alpha=(0-v_1)/(v_2-v_1)
        Scalar lphi=(*this)(lind[0],lind[1]);
        Scalar uphi=(*this)(uind[0],uind[1]);
        Scalar alpha=(level-lphi)/(uphi-lphi);

        //create the vertex
        Vector2 v(lind[0],lind[1]);
        v+=alpha*Vector2(dind[0],dind[1]);
        v[0]*=m_NI;
        v[1]*=m_NJ;
        verts.push_back(v);
        //this edge corresponds to the vertex at the end
        (*edgePtr)=verts.size()-1;

        //std::cout << ">";
    }
    //std::cout << *edgePtr << std::endl;
    return (*edgePtr);
}


//different triangles configurations associated with each vertex combination

//0 3
//1 2
template <typename GT>
const int MarchingSquaresGrid<GT>::m_edgeTable[16][5] = {
	{-1, -1, -1, -1, -1},//0000 0
	{ 0,  3, -1, -1, -1},//0001 1
	{ 1,  0, -1, -1, -1},//0010 2
	{ 1,  3, -1, -1, -1},//0011 3 
	{ 2,  1, -1, -1, -1},//0100 4
	{ 2,  3,  0,  1, -1},//0101 5 
	{ 2,  0, -1, -1, -1},//0110 6
	{ 2,  3, -1, -1, -1},//0111 7 
	{ 3,  2, -1, -1, -1},//1000 8 
	{ 0,  2, -1, -1, -1},//1001 9
	{ 3,  2,  1,  0, -1},//1010 10 
	{ 1,  2, -1, -1, -1},//1011 11
	{ 3,  1, -1, -1, -1},//1100 12 
	{ 0,  1, -1, -1, -1},//1101 13
	{ 3,  0, -1, -1, -1},//1110 14
	{-1, -1, -1, -1, -1},//1111 15
};

//template <typename GT>
//const int MarchingSquaresGrid<GT>::m_edgeTable[16][5] = {
//	{-1, -1, -1, -1, -1},//0000 0
//	{ 2,  3, -1, -1, -1},//0001 1
//	{ 2,  1, -1, -1, -1},//0010 2
//	{ 3,  1, -1, -1, -1},//0011 3 
//	{ 1,  0, -1, -1, -1},//0100 4
//	{ 3,  0,  1,  2, -1},//0101 5 
//	{ 2,  0, -1, -1, -1},//0110 6
//	{ 3,  0, -1, -1, -1},//0111 7 
//	{ 0,  3, -1, -1, -1},//1000 8 
//	{ 0,  2, -1, -1, -1},//1001 9
//	{ 0,  1,  2,  3, -1},//1010 10 
//	{ 0,  1, -1, -1, -1},//1011 11
//	{ 1,  3, -1, -1, -1},//1100 12 
//	{ 1,  2, -1, -1, -1},//1101 13
//	{ 2,  3, -1, -1, -1},//1110 14
//	{-1, -1, -1, -1, -1},//1111 15
//};
#endif

