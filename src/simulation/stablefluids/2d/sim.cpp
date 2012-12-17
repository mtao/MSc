#include "sim.h"
#include "solvers/linear/conjugate_gradient/pcg.hpp"
#include "simulation/utils/rand.h"

MACFluid2 MACStableFluids2::getState()
{
    MACFluid2 m;
    m.lphi = lphi.data();
    m.u = u.data();
    m.v = v.data();
    m.uw = uw.data();
    m.vw = vw.data();
    m.p = p.data();
    m.rhs = rhs.data();
    m.sphi = sphi.data();
    m.vorticity = vorticity.data();
    m.particles = particles;
    m.particle_radius = particle_radius;
    return m;
}

void MACStableFluids2::init()
{
    /*
    for(int j=0; j<u.NJ(); ++j)
        for(int i=0; i<u.NI(); ++i)
            u(i,j) = std::sin(3.14159 * i / float(u.NI()));
    for(int j=0; j<v.NJ(); ++j)
        for(int i=0; i<v.NI(); ++i)
            v(i,j) = std::sin(3.14159 * j / float(u.NJ()));
            */
    /*
    for(int i=2; i<u.NI()/5-2; ++i)
        for(int j=2; j<u.NJ()/5-2; ++j)
        {
    v(5*i,5*j)=10;
    v(5*i+1,5*j)=-10;
    u(5*i+1,5*j-1)=-10;
    u(5*i+1,5*j)=10;
        }
        */
    for(int i=0; i<size<NGrid>()*10; ++i)
    {
        float x=randhashf(i*2,0,1);
        float y=randhashf(i*2+1,0,1);
            particles.push_back(Vector2(x,y));
    }

}
void MACStableFluids2::step(Scalar dt)
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
auto MACStableFluids2::cfl() -> decltype(m_dx)
{
    return m_dx/std::max(
                u.mapVector().lpNorm<Eigen::Infinity>(),
                v.mapVector().lpNorm<Eigen::Infinity>()
                );
}
void MACStableFluids2::add_force(Scalar dt)
{
    return;
    foreach2d_ghost<VGrid>([&](int i, int j)
    {v(i,j) += -0.8*dt;});

}
void MACStableFluids2::project(Scalar dt)
{
    auto & d12_ghost = getD12_ghost();
    SparseMatrix L = d12_ghost * d12_ghost.transpose() ;
    Vector V(fluxSize());
    V.topRows(size<UGrid>()) = u.mapVector();
    V.bottomRows(size<VGrid>()) = v.mapVector();
    Vector rhs =  getD12()* V;
    MACStableFluids2::rhs.mapVector() = rhs;


    Vector pressure(rhs.rows());
    pressure.setZero();
    SparseCholeskyPreconditionedConjugateGradientSolve(L,rhs,pressure);
    p.mapVector()=pressure;
    Vector gradP = d12_ghost.transpose()*p.mapVector();
    uw.mapVector() = -gradP.topRows(size<UGrid>());
    vw.mapVector() = -gradP.bottomRows(size<VGrid>());
    u.mapVector() -= gradP.topRows(size<UGrid>());
    v.mapVector() -= gradP.bottomRows(size<VGrid>());

}
void MACStableFluids2::advect(Scalar dt)
{
    uold = u;
    vold = v;
    foreach2d<UGrid>(MACFluidAdvection::AdvectVelocity<UGrid,UGrid,VGrid>(u, uold, vold,dt));
    foreach2d<VGrid>(MACFluidAdvection::AdvectVelocity<VGrid,UGrid,VGrid>(v, uold, vold,dt));

}

void MACStableFluids2::advect_particles(Scalar dt)
{
    std::for_each(particles.begin(), particles.end(), [&](Vector2 & p)
    {
        MACFluidAdvection::advect_rk2(u,v,p,dt);
    });
}
void MACStableFluids2::diffuse(Scalar dt)
{
}

void MACStableFluids2::cursorPos(Vector2 vec)
{

    if(vec(0) < m_dx || vec(1) < m_dx || vec(0) > 1-m_dx || vec(1) > 1-m_dx)
        lastCursorPoll = 0;
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
    foreach2d_ghost<UGrid>([&](int i, int j)
    {
        u(i,j) += cursoru(i,j);
    });
    foreach2d_ghost<VGrid>([&](int i, int j)
    {
        v(i,j) += cursorv(i,j);
    });


}

