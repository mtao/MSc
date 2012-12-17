#ifndef MAC_GRID
#define MAC_GRID
#include "geometry/util/types.h"
#include <boost/serialization/serialization.hpp>
#include <iostream>
struct MACFluid
{
    typedef NumericalTraitsXf NumTraits;
    typedef typename NumTraits::Scalar Scalar;
    std::vector<NumTraits::Vector3> particles;
    NumTraits::Scalar particle_radius = 0.005;
    std::vector<Scalar>
    lphi;
    std::vector<Scalar>
    sphi;
    std::vector<Scalar>
    u;
    std::vector<Scalar>
    v;
    std::vector<Scalar>
    w;
    std::vector<Scalar>
    p;
    std::vector<Scalar>
    vorticity;
    std::vector<Scalar>
    rhs;
    std::vector<Scalar>
    uw;
    std::vector<Scalar>
    vw;
    std::vector<Scalar>
    ww;


    template <typename T>
    std::tuple<float *, int> toTuple(std::vector<T> & vec)
    {
        return std::tuple<float *, int>(reinterpret_cast<float *>(vec.data()), vec.size());
    }
    std::vector<std::tuple<float *,int> > getDataTuples()
    {
        typedef std::tuple<float *, int> Tuple;
        std::vector<Tuple> tuples;
        tuples.push_back(toTuple(lphi));//0
        tuples.push_back(toTuple(u));//1
        tuples.push_back(toTuple(v));//2
        tuples.push_back(toTuple(w));//3
        tuples.push_back(toTuple(uw));//4
        tuples.push_back(toTuple(vw));//5
        tuples.push_back(toTuple(ww));//6
        tuples.push_back(toTuple(p));//7
        tuples.push_back(toTuple(rhs));//8
        tuples.push_back(toTuple(sphi));//9
        tuples.push_back(toTuple(vorticity));//10
        tuples.push_back(toTuple(particles));//11
        std::get<1>(tuples.back()) = particles.size() * 3;
        return tuples;
    }
    virtual ~MACFluid(){}
};


/*TODO: Temporary data on the staggered grid size structure

        switch(type)
        {
            case GT_VERTEX:
                break;
            case GT_UFLUX:
                glDrawArrays(GL_POINTS, 0, (m_dim[0]+1)*(m_dim[1]));
                break;
            case GT_VFLUX:
                glDrawArrays(GL_POINTS, 0, (m_dim[0])*(m_dim[1]+1));
                break;
            case GT_CELL:
                glDrawArrays(GL_POINTS, 0, (m_dim[0])*(m_dim[1]));
                break;
            case GT_MARCHINGSQUARE:
                glDrawArrays(GL_POINTS, 0, (m_dim[0]+1)*(m_dim[1]+1));
                break;
        }

        */

/*
namespace boost { namespace serialization {
    template<class Archive>
        void serialize(Archive & ar, const MACGrid2 & t, unsigned int version)
        {

        }
}}
*/

#endif
