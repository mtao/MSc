
#ifndef STABLEFLUIDS_H
#define STABLEFLUIDS_H
#include "simulation/utils/mac2.h"
#include "geometry/levelset/mac/mac.h"

class MACStableFluids2: public MAC2f
{
public:
    typedef typename NumTraits::Scalar Scalar;
    typedef typename NumTraits::Vector Vector;
    typedef typename NumTraits::SparseMatrix SparseMatrix;
    typedef typename NumTraits::Vector2 Vector2;
    MACStableFluids2(uint i, uint j): MAC2f(i,j), m_dx(1/Scalar(i)), m_dxinv(Scalar(i))
      , lphi(grid<CGrid>()), sphi(grid<NGrid>())
      , u(grid<UGrid>()), v(grid<VGrid>())
      , vorticity(grid<NGrid>())
      , p(grid<CGrid>()), rhs(grid<CGrid>())
      , uw(grid<UGrid>()), vw(grid<VGrid>())
      , uold(grid<UGrid>()), vold(grid<VGrid>())
      , cursoru(grid<UGrid>()), cursorv(grid<VGrid>())
    {time=0;init();}

    virtual void init();
    virtual void step(Scalar dt);
    virtual void add_force(Scalar dt);
    virtual void diffuse(Scalar dt);
    virtual void advect(Scalar dt);
    virtual void advect_particles(Scalar dt);
    virtual void project(Scalar dt);
    Scalar cfl();
protected :
    Scalar m_dx, m_dxinv;
    UGrid uold;
    VGrid vold;

public:
    virtual void cursorPos(Vector2);
    MACFluid2 getState();
    void disablePoll() {lastCursorPoll=-1;}
    void enablePoll() {lastCursorPoll=0;}
protected:
    std::vector<NumTraits::Vector2> particles;
    NumTraits::Scalar particle_radius = 0.005;
    CGrid lphi;
    NGrid sphi;
    UGrid u;
    VGrid v;
    NGrid vorticity;
    CGrid p;
    CGrid rhs;
    UGrid uw;
    VGrid vw;
    Scalar lastCursorPoll;
    Scalar time;
    Vector2 lastCursorPos;
    UGrid cursoru;
    VGrid cursorv;

};

namespace MACFluidAdvection
{
template <class UGrid, class VGrid, class Vector>
void get_velocity(const UGrid & u, const VGrid & v, const Vector & p, Vector & vel)
{
    vel = Vector(u.lerp(p), v.lerp(p));
}

template <class UGrid, class VGrid, class Vector>
Vector get_velocity(const UGrid & u, const VGrid & v, const Vector & p)
{
    return Vector(u.lerp(p), v.lerp(p));
}

template <class UGrid, class VGrid, class Vector, typename Scalar>
void advect_rk2(const UGrid & u, const VGrid & v, Vector & p, Scalar dt)
{
    p += dt * get_velocity(u,v,(p+ 0.5*dt*get_velocity(u,v,p)).eval());

}

template <typename Grid, typename UGrid, typename VGrid>
struct AdvectVelocity
{
    static const int ind = std::is_same<Grid,VGrid>();
    typedef typename Grid::Scalar Scalar;
    typedef typename Grid::Vector2 Vector2;
    AdvectVelocity(Grid & g_, const UGrid & u_, const VGrid & v_, Scalar dt_):
        dt(dt_), g(g_), u(u_), v(v_) {}

    void operator()(int i,int j)
    {
        Vector2 p = g.vertex(i,j);
        advect_rk2(u,v,p,-dt);
        if(ind)
            g(i,j) = v.lerp(p);
        else
            g(i,j) = u.lerp(p);


    }
    Scalar dt;
    Grid & g;
    const UGrid & u;
    const VGrid & v;

};
};

template <typename Grid>
void printGrid(Grid & g)
{
        for(int j=0; j<g.NJ(); ++j)
        {
            for(int i=0; i<g.NI(); ++i)
                std::cout << g(i,j) << " ";
            std::cout << std::endl;
        }

}
template <typename Grid>
void printGridc(Grid & g)
{
    for(int j=0; j<g.NJ(); ++j)
    {
        for(int i=0; i<g.NI(); ++i)
            std::cout << (int)g(i,j) << " ";
        std::cout << std::endl;
    }

}

#endif
