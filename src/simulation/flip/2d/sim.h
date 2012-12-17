#ifndef FLIP_H
#define FLIP_H
#include "simulation/stablefluids_free_surface/2d/sim.h"

class MACFLIP2: public MACStableFluidsFreeSurface2
{
    public:
        typedef typename NumTraits::Scalar Scalar;
        typedef typename NumTraits::Vector Vector;
        typedef typename NumTraits::SparseMatrix SparseMatrix;
        typedef typename NumTraits::Vector2 Vector2;
        MACFLIP2(uint i, uint j): MACStableFluidsFreeSurface2(i,j), pu(grid<UGrid>()), pv(grid<VGrid>())
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
        std::vector<Vector2> particle_velocities;
        UGrid pu;
        VGrid pv;

        protected: Scalar Weight(Scalar x, Scalar y) ;



};
#endif
