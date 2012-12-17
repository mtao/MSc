#ifndef FLIP_H
#define FLIP_H
#include "simulation/stablefluids_free_surface/3d/sim.h"

class MACFLIP: public MACStableFluidsFreeSurface
{
    public:
        typedef typename NumTraits::Scalar Scalar;
        typedef typename NumTraits::Vector Vector;
        typedef typename NumTraits::SparseMatrix SparseMatrix;
        typedef typename NumTraits::Vector3 Vector3;
        typedef typename NumTraits::Vector2 Vector2;
        MACFLIP(uint i, uint j,uint k): MACStableFluidsFreeSurface(i,j,k)
          , pu(grid<UGrid>())
          , pv(grid<VGrid>())
          , pw(grid<WGrid>())
        {init();}

        virtual void step(Scalar dt);
        virtual void cursorPos(Vector2 vec);
       protected:
        virtual void init();
        virtual Scalar cfl();
        virtual void advect(Scalar dt);
        virtual void advect_particles(Scalar dt);
        virtual void add_force(Scalar dt);





    protected:
        std::vector<Vector3> particle_velocities;
        UGrid pu;
        VGrid pv;
        WGrid pw;

        protected: Scalar Weight(Scalar x, Scalar y, Scalar z) ;
template <typename Grid, typename Scalar, typename Vector>
void add_weight(Grid & u, Grid & uw, Vector & vel,
                uint i,Scalar fi,uint j,Scalar fj,uint k,Scalar fk,
                int pi, int pj, int pk, int dim)
{
    Scalar weight = Weight(
                pi==1?1-fi:fi,
                pj==1?1-fj:fj,
                pk==1?1-fk:fk);
    u(i+pi,j+pj,k+pk) += weight * vel(dim);
    uw(i+pi,j+pj,k+pk) += weight;
}



};
#endif
