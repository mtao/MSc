#include "sim.h"
#include "simulation/flip/3d/particledensitycorrector.hpp"

#include "solvers/quadratic/mprgp/mprgp.hpp"
//#include "solvers/linear/conjugate_gradient/pcg.hpp"


/*
auto UIC::cfl() -> decltype(m_dx)
{
    Scalar dx = std::max(m_dx,particle_radius*Scalar(4));
    return dx/std::max(
//    return m_dx/std::max(
                u.mapVector().lpNorm<Eigen::Infinity>(),
                v.mapVector().lpNorm<Eigen::Infinity>()
                );
}
*/
void UIC::init()
{

    MACFLIP::init();

    for(int i=0; i < sphi.size() ; ++i)
        sphi(i) += m_dx;
    pressure = Vector(size<CGrid>());
    pressure.setZero();
    u.fill(0);
    v.fill(0);
}

/*
void UIC::add_force(Scalar dt)
{

    MACStableFluidsFreeSurface::add_force(dt);
    std::for_each(particle_velocities.begin(), particle_velocities.end(), [&](Vector3 & p)
    {p(1) += -0.8*dt;});
}

*/
void UIC::step(Scalar dt)
{
    float t=0;
    int count=0;
    while(t<dt)
    {
        ParticleDensityCorrector<NumTraits, CGrid> pdc(particles, particle_velocities,lphi, particle_radius);
        float substep=cfl();

        if(substep < 0.00001)
        {
            std::cout << "VERY LOW SUBSTEP";
        }
        if(t+substep > dt)
            substep=dt-t;


        pdc.twiddleParticles();
        add_force(dt);
        //printGrid(v);
        advect(dt);
        pu = u;
        pv = v;
        pw = w;
        project(dt);
        advect_particles(dt);



        ++count;
        t+=substep;
    }
    time += dt;
    compute_liquid_phi(lphi);
}


void UIC::project(Scalar dt)
{
    auto & d23 = getD23();
        compute_uvw_weights();
    lphi.fill(0);
    std::for_each(particles.begin(), particles.end(), [&](Vector3 & p)
    {
        Scalar fi, fj,fk;
        uint i,j,k;
        sphi.barycentric(p,i,fi,j,fj,k,fk);
        //lphi(i,j) += m_dxinv*m_dxinv * std::sqrt(12)* particle_radius*particle_radius / 2;
        // (4/3)pi r^3/(8*dx * dx * dx* pi/sqrt(18))
        lphi(i,j,k) += m_dxinv*m_dxinv*m_dxinv * particle_radius*particle_radius*particle_radius/std::sqrt(2)/.95;
    });
    SparseMatrix L = d23 * d23.transpose() ;

    MACStableFluids::rhs.mapVector() = 1-lphi.mapVector().array();

    foreach3d<decltype(rhs)>([&](int i, int j, int k)
    {
        if(sphi.lerp(MACStableFluids::rhs.vertex(i,j,k)) < 0)
            MACStableFluids::rhs(i,j,k)-=1;
    });

    Vector rhs = -MACStableFluids::rhs.mapVector();

    Eigen::VectorXd drhs = rhs.cast<double>();
    Eigen::VectorXd dpressure = pressure.cast<double>();
    Eigen::SparseMatrix<double> dL = L.cast<double>();
    SparseMPRGPSolve(dL,drhs,dpressure);
    rhs = drhs.cast<Scalar>();
    pressure = dpressure.cast<Scalar>();
    //pressure *= m_dx;
    p.mapVector()=pressure;
    Vector gradP = -d23.transpose()*p.mapVector();
    /*
    uw.mapVector() = -gradP.topRows(size<UGrid>());
    vw.mapVector() = -gradP.bottomRows(size<VGrid>());
    */
    u.mapVector() -= gradP.segment(matIndex<UGrid>(0,0,0),size<UGrid>());
    v.mapVector() -= gradP.segment(matIndex<VGrid>(0,0,0),size<VGrid>());
    w.mapVector() -= gradP.segment(matIndex<WGrid>(0,0,0),size<WGrid>());
}

void UIC::advect_particles(Scalar dt)
{
    for(int i=0; i < u.size(); ++i) pu(i) = u(i) - pu(i);
    for(int i=0; i < v.size(); ++i) pv(i) = v(i) - pv(i);
    for(int i=0; i < w.size(); ++i) pw(i) = w(i) - pw(i);
    for(int i=0; i<particles.size(); ++i)
    {
        Vector3 & p = particles[i];
        Vector3 & vel = particle_velocities[i];

        vel +=
                Vector3(pu.lerp(p), pv.lerp(p),pw.lerp(p));
        MACFluidAdvection::advect_rk2(u,v,w,p,dt);
        //particles[i] += particle_velocities[i]*dt; //A terrible move

        Scalar psphi = sphi.lerp(p);
        if(psphi < 0)
        {
            Vector3 normal;
            sphi.gradient(p,normal);
            normal.normalize();
            p -= psphi * normal;
            vel -= vel.dot(normal)*normal;

        }


    }
}

