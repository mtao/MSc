#ifndef UIC_H
#define UIC_H
#include "simulation/flip/3d/sim.h"
//#include "simulation/stablefluids_density/2d/sim.h"
#define FLIP
#ifdef FLIP
class UIC: public MACFLIP
        #else

class UIC: public MACStableFluidsFreeSurface2
        #endif
{
public:
    typedef typename NumTraits::Scalar Scalar;
    typedef typename NumTraits::Vector Vector;
    typedef typename NumTraits::SparseMatrix SparseMatrix;
    typedef typename NumTraits::Vector3 Vector3;
    UIC(uint i, uint j, uint k): MACFLIP(i,j,k)
    {init();}

    virtual void init();
    virtual void step(Scalar dt);
protected:
    //virtual Scalar cfl();
    virtual void project(Scalar dt);
    virtual void advect_particles(Scalar dt);
    Vector pressure;




protected:



};
#endif
