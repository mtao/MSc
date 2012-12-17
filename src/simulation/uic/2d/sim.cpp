#include "sim.h"
#include "simulation/flip/2d/particledensitycorrector.hpp"

#include "solvers/quadratic/mprgp/mprgp.hpp"
//#include "solvers/linear/conjugate_gradient/pcg.hpp"


/*
auto UIC2::cfl() -> decltype(m_dx)
{
    Scalar dx = std::max(m_dx,particle_radius*Scalar(4));
    return dx/std::max(
//    return m_dx/std::max(
                u.mapVector().lpNorm<Eigen::Infinity>(),
                v.mapVector().lpNorm<Eigen::Infinity>()
                );
}
*/
void UIC2::init()
{
#ifdef FLIP

    MACFLIP2::init();
#else
    MACStableFluidsFreeSurface2::init();
#endif

    for(int i=0; i < sphi.size() ; ++i)
        sphi(i) += m_dx;
    pressure = Vector(size<CGrid>());
    pressure.setZero();
    u.fill(0);
    v.fill(0);
}

void UIC2::add_force(Scalar dt)
{

    MACStableFluidsFreeSurface2::add_force(dt);
    std::for_each(particle_velocities.begin(), particle_velocities.end(), [&](Vector2 & p)
    {p(1) += -0.8*dt;});
}

void UIC2::step(Scalar dt)
{
    float t=0;
    int count=0;
    while(t<dt)
    {
        ParticleDensityCorrector<NumTraits, CGrid> pdc(particles, particle_velocities,lphi, particle_radius);
        float substep=cfl();

        if(t+substep > dt)
            substep=dt-t;


        pdc.twiddleParticles();
        add_force(dt);
        //printGrid(v);
        advect(dt);
        pu = u;
        pv = v;
        project(dt);
        advect_particles(dt);



        ++count;
        t+=substep;
    }
    time += dt;
    compute_liquid_phi(lphi);
}

template <typename Grid>
void clearGrid(Grid & grid)
{
    for(int i=0; i<grid.NI(); ++i)
    {
        grid(i,0) =0.0;
        grid(i,grid.NJ()-1) =0.0;
    }
    for(int j=1; j < grid.NJ()-1; ++j)
    {
        grid(0,j) =0;
        grid(grid.NI()-1,j) =0;
    }
}

void UIC2::project(Scalar dt)
{
    auto & d12_ghost = getD12_ghost();
        compute_uv_weights();
    lphi.fill(0);
    std::for_each(particles.begin(), particles.end(), [&](Vector2 & p)
    {
        Scalar fi, fj;
        uint i,j;
        sphi.barycentric(p,i,fi,j,fj);
        // pi r^2(4* dx * dx* (pi/2 r^2) / (sqrt(3) r^2))
        // pi r^2/(4* dx * dx* pi/sqrt(12))
        lphi(i,j) += m_dxinv*m_dxinv * std::sqrt(3)* particle_radius*particle_radius/2/.95;
    });
    SparseMatrix L = d12_ghost * d12_ghost.transpose() ;
    MACStableFluids2::rhs.mapVector() = 1-lphi.mapVector().array();
    foreach2d<decltype(rhs)>([&](int i, int j)
    {
        if(sphi.lerp(MACStableFluids2::rhs.vertex(i,j)) < 0)
            MACStableFluids2::rhs(i,j)-=1;
    });

    Vector rhs = -MACStableFluids2::rhs.mapVector();

    Eigen::VectorXd drhs = rhs.cast<double>();
    Eigen::VectorXd dpressure = pressure.cast<double>();
    Eigen::SparseMatrix<double> dL = L.cast<double>();
    SparseMPRGPSolve(dL,drhs,dpressure);
    rhs = drhs.cast<Scalar>();
    pressure = dpressure.cast<Scalar>();
    //pressure *= m_dx;
    p.mapVector()=pressure;
    Vector gradP = -d12_ghost.transpose()*p.mapVector();
    /*
    uw.mapVector() = -gradP.topRows(size<UGrid>());
    vw.mapVector() = -gradP.bottomRows(size<VGrid>());
    */
    u.mapVector() -= gradP.topRows(size<UGrid>());
    v.mapVector() -= gradP.bottomRows(size<VGrid>());
}

void UIC2::advect_particles(Scalar dt)
{
    for(int i=0; i < u.size(); ++i) pu(i) = u(i) - pu(i);
    for(int i=0; i < v.size(); ++i) pv(i) = v(i) - pv(i);
    for(int i=0; i<particles.size(); ++i)
    {
        Vector2 & p = particles[i];
        Vector2 & vel = particle_velocities[i];

        vel +=
                Vector2(pu.lerp(p), pv.lerp(p));
        MACFluidAdvection::advect_rk2(u,v,p,dt);
        //particles[i] += particle_velocities[i]*dt; //A terrible move
        p(0) = std::min(1-m_dx,p(0));
        p(1) = std::min(1-m_dx,p(1));
        p(0) = std::max(m_dx,p(0));
        p(1) = std::max(m_dx,p(1));

        Scalar psphi = sphi.lerp(p);
        if(psphi < 0)
        {
            Vector2 normal;
            sphi.gradient(p,normal);
            normal.normalize();
            p -= psphi * normal;
            vel -= vel.dot(normal)*normal;

        }


    }
}

