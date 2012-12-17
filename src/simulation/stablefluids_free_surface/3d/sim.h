#ifndef STABLEFLUIDSFREESURFACE_H
#define STABLEFLUIDSFREESURFACE_H
#include "simulation/stablefluids/3d/sim.h"

class MACStableFluidsFreeSurface: public MACStableFluids
{
    public:
        typedef typename NumTraits::Scalar Scalar;
        typedef typename NumTraits::Vector Vector;
        typedef typename NumTraits::SparseMatrix SparseMatrix;
        typedef typename NumTraits::Vector3 Vector3;
        MACStableFluidsFreeSurface(uint i, uint j, uint k): MACStableFluids(i,j, k),
            uv(grid<UGridc>()),
            vv(grid<VGridc>()),
            wv(grid<WGridc>())

        {init();}

        virtual void init();
        virtual void step(Scalar dt);
        virtual void diffuse(Scalar dt);
        virtual void advect_particles(Scalar dt);
        virtual void project(Scalar dt);
    virtual void add_force(Scalar dt);


        //Phi stuff
        virtual void compute_phi(CGrid & phi);
        virtual void compute_liquid_phi(CGrid & phi);
        virtual void compute_solid_lphi(CGrid & phi);
        virtual void compute_uvw_weights();
        virtual void constrain_velocity();
        template< typename GridType, typename GridValidType>
        void extrapolate(GridType & grid, GridValidType & valid);

    protected:
        UGridc uv;
        VGridc vv;
        WGridc wv;



};
    template< typename GridType, typename GridValidType>
void MACStableFluidsFreeSurface::extrapolate(GridType& grid, GridValidType & valid)
{
    GridValidType old_valid = valid;
    int count;
    Scalar sum;
    for(int layers=0; layers < 3; ++layers)
    {
        old_valid = valid;
        for(int k=1; k < grid.NK()-1; ++k) for(int j=1; j< grid.NJ()-1; ++j) for(int i=1; i< grid.NI()-1; ++i)
        {
            sum = 0;
            count=0;
            if(!old_valid(i,j,k))
            {
                if(old_valid(i+1,j,k))
                {
                    sum+= grid(i+1,j,k);
                    ++count;
                }
                if(old_valid(i-1,j,k))
                {
                    sum+= grid(i-1,j,k);
                    ++count;
                }
                if(old_valid(i,j+1,k))
                {
                    sum+= grid(i,j+1,k);
                    ++count;
                }
                if(old_valid(i,j-1,k))
                {
                    sum+= grid(i,j-1,k);
                    ++count;
                }
                if(old_valid(i,j,k+1))
                {
                    sum+= grid(i,j,k+1);
                    ++count;
                }
                if(old_valid(i,j,k-1))
                {
                    sum+= grid(i,j,k-1);
                    ++count;
                }
            }
            if(count>0)
            {
                grid(i,j,k) = sum/count;
                valid(i,j,k)=1;
            }





        }

    }

}

#endif
