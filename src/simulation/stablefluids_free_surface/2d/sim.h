#ifndef STABLEFLUIDSFREESURFACE_H
#define STABLEFLUIDSFREESURFACE_H
#include "simulation/stablefluids/2d/sim.h"

class MACStableFluidsFreeSurface2: public MACStableFluids2
{
    public:
        typedef typename NumTraits::Scalar Scalar;
        typedef typename NumTraits::Vector Vector;
        typedef typename NumTraits::SparseMatrix SparseMatrix;
        typedef typename NumTraits::Vector2 Vector2;
        MACStableFluidsFreeSurface2(uint i, uint j): MACStableFluids2(i,j),
            uv(grid<UGridc>()),
            vv(grid<VGridc>()),
            pressure(size<CGrid>())

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
        virtual void compute_uv_weights();
        virtual void constrain_velocity();
        template< typename GridType, typename GridValidType>
        void extrapolate(GridType & grid, GridValidType & valid);

    protected:
        UGridc uv;
        VGridc vv;
    Eigen::VectorXd pressure;



};
    template< typename GridType, typename GridValidType>
void MACStableFluidsFreeSurface2::extrapolate(GridType& grid, GridValidType & valid)
{
    GridValidType old_valid = valid;
    int count;
    Scalar sum;
    for(int layers=0; layers < 4; ++layers)
    {
        old_valid = valid;
        for(int j=grid.NJ()-2; j>0; --j) for(int i=grid.NI()-2; i>0; --i)
        //for(int j=1; j< grid.NJ()-1; ++j) for(int i=1; i< grid.NI()-1; ++i)
        {
            sum = 0;
            count=0;
            if(!old_valid(i,j))
            {
                if(old_valid(i+1,j))
                {
                    sum+= grid(i+1,j);
                    ++count;
                }
                if(old_valid(i-1,j))
                {
                    sum+= grid(i-1,j);
                    ++count;
                }
                if(old_valid(i,j+1))
                {
                    sum+= grid(i,j+1);
                    ++count;
                }
                if(old_valid(i,j-1))
                {
                    sum+= grid(i,j-1);
                    ++count;
                }
            }
            if(count>0)
            {
                grid(i,j) = sum/count;
                valid(i,j)=1;
            }





        }

    }

}

#endif
