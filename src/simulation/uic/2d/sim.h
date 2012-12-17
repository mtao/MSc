#ifndef UIC_H
#define UIC_H
#include "simulation/flip/2d/sim.h"
//#include "simulation/stablefluids_density/2d/sim.h"
#define FLIP
#ifdef FLIP
class UIC2: public MACFLIP2
        #else

class UIC2: public MACStableFluidsFreeSurface2
        #endif
{
public:
    typedef typename NumTraits::Scalar Scalar;
    typedef typename NumTraits::Vector Vector;
    typedef typename NumTraits::SparseMatrix SparseMatrix;
    typedef typename NumTraits::Vector2 Vector2;
#ifdef FLIP
    UIC2(uint i, uint j): MACFLIP2(i,j)
  #else
    UIC2(uint i, uint j): MACStableFluidsFreeSurface2(i,j)
  #endif
      //UIC2(uint i, uint j): MACStableFluidsDensity2(i,j)
    {init();}

    virtual void init();
    virtual void step(Scalar dt);
protected:
    //virtual Scalar cfl();
    virtual void project(Scalar dt);
    virtual void advect_particles(Scalar dt);
    virtual void add_force(Scalar dt);
    Vector pressure;




protected:



};
#endif
