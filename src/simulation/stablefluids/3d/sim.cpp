#include "sim.h"
#include "solvers/linear/conjugate_gradient/pcg.hpp"
#include "simulation/utils/rand.h"


MACFluid MACStableFluids::getState()
{
    MACFluid m;
    m.lphi = lphi.data();
    UGrid u(grid<UGrid>());
    /*
#define UGRID
#define VGRID
#define WGRID
*/
#ifdef UGRID
    foreach3d<UGrid>([&](int i, int j, int k)
    {
        u(i,j,k) = 1;
        std::cout << matIndex<UGrid>(i,j,k) << ":[" << i << " " << j << " " << k << "]" << std::endl;
    });
    std::cout << u.NI() << " " << u.NJ() << " " << u.NK() << std::endl;
#endif
#ifdef VGRID
    VGrid v(grid<VGrid>());
    foreach3d<VGrid>([&](int i, int j, int k)
    {
        v(i,j,k) = 1;
        std::cout << matIndex<VGrid>(i,j,k) << ":[" << i << " " << j << " " << k << "]" << std::endl;
    });
    std::cout << v.NI() << " " << v.NJ() << " " << v.NK() << std::endl;
#endif

#ifdef WGRID
    WGrid w(grid<WGrid>());
    foreach3d<WGrid>([&](int i, int j, int k)
    {
        w(i,j,k) = 1;
        std::cout << matIndex<WGrid>(i,j,k) << ":[" << i << " " << j << " " << k << "]" << std::endl;
    });
    std::cout << w.NI() << " " << w.NJ() << " " << w.NK() << std::endl;
#endif
    m.u = u.data();
    m.v = v.data();
    m.w = w.data();
    m.uw = uw.data();
    m.vw = vw.data();
    m.ww = ww.data();
    m.p = p.data();
    m.rhs = rhs.data();
    m.sphi = sphi.data();
    m.vorticity = vorticity.data();
    m.particles = particles;
    m.particle_radius = particle_radius;
    return m;
}

void MACStableFluids::init()
{
    /*
    for(int j=0; j<u.NJ(); ++j)
        for(int i=0; i<u.NI(); ++i)
            u(i,j) = std::sin(3.14159 * i / float(u.NI()));
    for(int j=0; j<v.NJ(); ++j)
        for(int i=0; i<v.NI(); ++i)
            v(i,j) = std::sin(3.14159 * j / float(u.NJ()));
            */
    for(int i=2; i<u.NI()/3-2; ++i)
        for(int j=2; j<u.NJ()/3-2; ++j)
        {
    v(3*i,3*j,u.NK()/2)=10;
    v(3*i+1,3*j,u.NK()/2)=-10;
    u(3*i+1,3*j-1,u.NK()/2)=-10;
    u(3*i+1,3*j,u.NK()/2)=10;
        }
    u(u.NI()/2, u.NJ()/2,u.NK()/2 )=10;
    for(int i=0; i<size<NGrid>(); ++i)
    {
        float x=randhashf(i*3,0,1);
        float y=randhashf(i*3+1,0,1);
        float z=randhashf(i*3+2,0,1);
            particles.push_back(Vector3(x,y,z));
    }


}
void MACStableFluids::step(Scalar dt)
{
    float t=0;
    int count=0;
    while(t<dt)
    {
        float substep=cfl();

        if(substep<0.000001)
        {
            std::cerr << "SUBSTEP GOING TOO LOW QUITTING\n";
            return;
        }
        if(t+substep > dt)
            substep=dt-t;


        add_force(dt);
        advect(dt);
        diffuse(dt);
        project(dt);
        advect_particles(dt);
        ++count;
        t+=substep;
    }
    time += dt;


}
auto MACStableFluids::cfl() -> decltype(m_dx)
{
    return m_dx/std::max(std::max(
                u.mapVector().lpNorm<Eigen::Infinity>(),
                v.mapVector().lpNorm<Eigen::Infinity>()),
                w.mapVector().lpNorm<Eigen::Infinity>()
                );
}
void MACStableFluids::add_force(Scalar dt)
{
    foreach3d_ghost<VGrid>([&](int i, int j, int k)
    {v(i,j,k) += -0.8*dt;});

}
void MACStableFluids::project(Scalar dt)
{
    auto & d23_ghost = getD23_ghost();
    SparseMatrix L = d23_ghost * d23_ghost.transpose() ;
    Vector V(fluxSize());
    V.topRows(size<UGrid>()) = u.mapVector();
    V.segment(size<UGrid>(),size<VGrid>()) = v.mapVector();
    V.bottomRows(size<WGrid>()) = w.mapVector();
    Vector rhs =  getD23()* V;
    MACStableFluids::rhs.mapVector() = rhs;


    Vector pressure(rhs.rows());
    pressure.setZero();
    SparseCholeskyPreconditionedConjugateGradientSolve(L,rhs,pressure);
    p.mapVector()=pressure;
    Vector gradP = d23_ghost.transpose()*p.mapVector();
    uw.mapVector() = -gradP.topRows(size<UGrid>());
    vw.mapVector() = -gradP.segment(size<UGrid>(), size<VGrid>());
    ww.mapVector() = -gradP.bottomRows(size<WGrid>());
    u.mapVector() -= gradP.topRows(size<UGrid>());
    v.mapVector() -= gradP.segment(size<UGrid>(), size<VGrid>());
    w.mapVector() -= gradP.bottomRows(size<WGrid>());

}
void MACStableFluids::advect(Scalar dt)
{
    uold = u;
    vold = v;
    wold = w;
    foreach3d<UGrid>(MACFluidAdvection::AdvectVelocity<UGrid,UGrid,VGrid,WGrid>(u, uold, vold,wold,dt));
    foreach3d<VGrid>(MACFluidAdvection::AdvectVelocity<VGrid,UGrid,VGrid,WGrid>(v, uold, vold,wold,dt));
    foreach3d<WGrid>(MACFluidAdvection::AdvectVelocity<WGrid,UGrid,VGrid,WGrid>(w, uold, vold,wold,dt));

}

void MACStableFluids::advect_particles(Scalar dt)
{
    std::for_each(particles.begin(), particles.end(), [&](Vector3 & p)
    {
        MACFluidAdvection::advect_rk2(u,v,w,p,dt);
    });
}
void MACStableFluids::diffuse(Scalar dt)
{
}

void MACStableFluids::cursorPos(Vector2 vec)
{

    return;
    /*
    cursoru.fill(0);
    cursorv.fill(0);
    if(lastCursorPoll>0.0)
    {
        Vector2 vel = 1000*(time - lastCursorPoll) * (vec - lastCursorPos);
        uint i1,j1,i2,j2; Scalar fi,fj;
        u.barycentric(vec,i1,fi,j1,fj);
        u.barycentric(lastCursorPos,i2,fi,j2,fj);
        if(i1>i2){i1 ^= i2; i2 ^= i1; i1 ^= i2;}
        if(j1>j2){j1 ^= j2; j2 ^= j1; j1 ^= j2;}
        for(int i=i1; i<=i2; ++i)
        {
            for(int j=j1; j<=j2; ++j)
            {
                cursoru(i,j) += vel(0);
            }
        }
        v.barycentric(vec,i1,fi,j1,fj);
        v.barycentric(lastCursorPos,i2,fi,j2,fj);
        if(i1>i2){i1 ^= i2; i2 ^= i1; i1 ^= i2;}
        if(j1>j2){j1 ^= j2; j2 ^= j1; j1 ^= j2;}
        for(int i=i1; i<=i2; ++i)
        {
            for(int j=j1; j<=j2; ++j)
            {
                cursorv(i,j) += vel(1);
            }
        }



    }
    lastCursorPos=vec;
    lastCursorPoll=time;
    foreach3d2d_ghost<UGrid>([&](int i, int j)
    {
        u(i,j) += cursoru(i,j);
    });
    foreach3d2d_ghost<VGrid>([&](int i, int j)
    {
        v(i,j) += cursorv(i,j);
    });
    */


}

