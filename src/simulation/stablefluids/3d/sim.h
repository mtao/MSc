
#ifndef STABLEFLUIDS_H
#define STABLEFLUIDS_H
#include "simulation/utils/mac.h"
#include "geometry/levelset/mac/mac.h"

class MACStableFluids: public MACf
{
public:
    typedef typename NumTraits::Scalar Scalar;
    typedef typename NumTraits::Vector Vector;
    typedef typename NumTraits::SparseMatrix SparseMatrix;
    typedef typename NumTraits::Vector3 Vector3;
    typedef typename NumTraits::Vector2 Vector2;
    MACStableFluids(uint i, uint j, uint k): MACf(i,j,k), m_dx(1/Scalar(i)), m_dxinv(Scalar(i))
      , lphi(grid<CGrid>()), sphi(grid<NGrid>())
      , u(grid<UGrid>()), v(grid<VGrid>()), w(grid<WGrid>())
      , vorticity(grid<NGrid>())
      , p(grid<CGrid>()), rhs(grid<CGrid>())
      , uw(grid<UGrid>()), vw(grid<VGrid>()), ww(grid<WGrid>())
      , uold(grid<UGrid>()), vold(grid<VGrid>()), wold(grid<WGrid>())
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
    WGrid wold;

public:
    virtual void cursorPos(Vector2);
    MACFluid getState();
    void disablePoll() {lastCursorPoll=-1;}
    void enablePoll() {lastCursorPoll=0;}
protected:
    std::vector<NumTraits::Vector3> particles;
    NumTraits::Scalar particle_radius = 0.005;
    CGrid lphi;
    NGrid sphi;
    UGrid u;
    VGrid v;
    WGrid w;
    NGrid vorticity;
    CGrid p;
    CGrid rhs;
    UGrid uw;
    VGrid vw;
    WGrid ww;
    Scalar lastCursorPoll;
    Scalar time;
//    Vector2 lastCursorPos;
    UGrid cursoru;
    VGrid cursorv;

};

namespace MACFluidAdvection
{
template <class UGrid, class VGrid, class WGrid, class Vector>
void get_velocity(const UGrid & u, const VGrid & v, const WGrid & w, const Vector & p, Vector & vel)
{
    vel = Vector(u.lerp(p), v.lerp(p), w.lerp(p));
}

template <class UGrid, class VGrid, class WGrid, class Vector>
Vector get_velocity(const UGrid & u, const VGrid & v, const WGrid & w, const Vector & p)
{
    return Vector(u.lerp(p), v.lerp(p), w.lerp(p));
}

template <class UGrid, class VGrid, class WGrid, class Vector, typename Scalar>
void advect_rk2(const UGrid & u, const VGrid & v, const WGrid & w, Vector & p, Scalar dt)
{
    p += dt * get_velocity(u,v,w,(p+ 0.5*dt*get_velocity(u,v,w,p)).eval());

}

template <typename Grid, typename UGrid, typename VGrid, typename WGrid>
struct AdvectVelocity
{
    static const int isu = std::is_same<Grid,UGrid>();
    static const int isv = std::is_same<Grid,VGrid>();
    typedef typename Grid::Scalar Scalar;
    typedef typename Grid::Vector3 Vector3;
    AdvectVelocity(Grid & g_, const UGrid & u_, const VGrid & v_, const WGrid & w_, Scalar dt_):
        dt(dt_), g(g_), u(u_), v(v_), w(w_) {}

    void operator()(int i,int j, int k)
    {
        Vector3 p = g.vertex(i,j,k);
        advect_rk2(u,v,w,p,-dt);
        auto f = g(i,j,k);
        if(isu)
            g(i,j,k) = u.lerp(p);
        else if(isv)
            g(i,j,k) = v.lerp(p);
        else
            g(i,j,k) = w.lerp(p);


    }
    Scalar dt;
    Grid & g;
    const UGrid & u;
    const VGrid & v;
    const WGrid & w;

};
};

template <typename Grid>
void printGrid(Grid & g)
{
        for(int k=0; k<g.NK(); ++k)
        for(int j=0; j<g.NJ(); ++j)
        {
            for(int i=0; i<g.NI(); ++i)
                std::cout << g(i,j,k) << " ";
            std::cout << std::endl;
        }

}
template <typename Grid>
void printGridc(Grid & g)
{
        for(int k=0; k<g.NK(); ++k)
    for(int j=0; j<g.NJ(); ++j)
    {
        for(int i=0; i<g.NI(); ++i)
            std::cout << (int)g(i,j,k) << " ";
        std::cout << std::endl;
    }

}

#endif
