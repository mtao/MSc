#include "sim.h"
#include "particledensitycorrector.hpp"

void MACFLIP2::init()
{
    MACStableFluidsFreeSurface2::init();
    particle_velocities.resize(particles.size(), Vector2(0,0));
}


void MACFLIP2::add_force(Scalar dt)
{
    MACStableFluidsFreeSurface2::add_force(dt);
    std::for_each(particle_velocities.begin(), particle_velocities.end(), [&](Vector2 & p)
    {p(1) += -0.8*dt;});

}
void MACFLIP2::advect_particles(Scalar dt)
{
    for(int i=0; i < u.size(); ++i) pu(i) = u(i) - pu(i);
    for(int i=0; i < v.size(); ++i) pv(i) = v(i) - pv(i);
    for(int i=0; i<particles.size(); ++i)
    {

        particle_velocities[i] +=
                Vector2(pu.lerp(particles[i]), pv.lerp(particles[i]));
        MACFluidAdvection::advect_rk2(u,v,particles[i],dt);
        //particles[i] += particle_velocities[i]*dt; //A terrible move

        Scalar psphi = sphi.lerp(particles[i])+particle_radius;
        if(psphi < 0)
        {
            Vector2 normal;
            sphi.gradient(particles[i],normal);
            normal.normalize();
            particle_velocities[i] -= particle_velocities[i].dot(normal) * normal;
            particles[i] -= psphi * normal;

        }
    }
}

auto MACFLIP2::cfl() -> decltype(m_dx)
{
    return m_dx/std::max(
                u.mapVector().lpNorm<Eigen::Infinity>(),
                v.mapVector().lpNorm<Eigen::Infinity>()
                );
}

void MACFLIP2::step(Scalar dt)
{
    float t=0;
    int count=0;
    while(t<dt)
    {
        float substep=cfl();

        if(substep<0.0001)
        {
            std::cerr << "SUBSTEP GOING TOO LOW QUITTING\n";
            return;
        }
        if(t+substep > dt)
            substep=dt-t;
        std::cout << "Substep.\n";


        add_force(dt);
        advect(dt);
        pu = u;
        pv = v;

        //standard sim stuff
        diffuse(dt);
        project(dt);//uw and vw are set here
        extrapolate(u,uv);
        extrapolate(v,vv);
        constrain_velocity();



        advect_particles(dt);
        ParticleDensityCorrector<NumTraits, CGrid> pdc(particles, particle_velocities,lphi, particle_radius);
        pdc.twiddleParticles();


        ++count;
        t+=substep;
    }
    time += dt;
    compute_liquid_phi(lphi);


}
auto MACFLIP2::Weight(Scalar x, Scalar y) -> Scalar
{
    return std::max(
                Scalar(0),
                    std::pow(
                        1
                    -(x*x+y*y),Scalar(3)
                        )
                    );
}

void MACFLIP2::advect(Scalar dt)
{
    uw.fill(0);
    vw.fill(0);
    u.fill(0);
    v.fill(0);
    Scalar fi,fj;
    uint i,j;
    Scalar weight;
    for(int k=0; k<particles.size(); ++k)
    {
        const Vector2 & p = particles[k];
        const Vector2 & vel = particle_velocities[k];

        u.barycentric(p,i,fi,j,fj);

        weight = Weight(fi,fj);
        u(i,j) += weight * vel(0);
        uw(i,j) += weight;

        weight = Weight(1-fi,fj);
        u(i+1,j) += weight * vel(0);
        uw(i+1,j) += weight;

        weight = Weight(fi,1-fj);
        u(i,j+1) += weight * vel(0);
        uw(i,j+1) += weight;

        weight = Weight(1-fi,1-fj);
        u(i+1,j+1) += weight * vel(0);
        uw(i+1,j+1) += weight;

        v.barycentric(p,i,fi,j,fj);

        weight = Weight(fi,fj);
        v(i,j) += weight * vel(1);
        vw(i,j) += weight;

        weight = Weight(1-fi,fj);
        v(i+1,j) += weight * vel(1);
        vw(i+1,j) += weight;

        weight = Weight(fi,1-fj);
        v(i,j+1) += weight * vel(1);
        vw(i,j+1) += weight;

        weight = Weight(1-fi,1-fj);
        v(i+1,j+1) += weight * vel(1);
        vw(i+1,j+1) += weight;
    }
    foreach2d_ghost<UGrid>([&](int i, int j)
    {
        if(uw(i,j)>0)
            u(i,j) /= uw(i,j);
    });
    foreach2d_ghost<VGrid>([&](int i, int j)
    {
        if(vw(i,j)>0)
            v(i,j) /= vw(i,j);
    });
    foreach_boundary<UGrid>([&](int i,int j)
    {
        u(i,j) = 0;
    });
    foreach_boundary<VGrid>([&](int i,int j)
    {
        v(i,j) = 0;
    });
}

void MACFLIP2::cursorPos(Vector2 vec)
{
    MACStableFluids2::cursorPos(vec);
    for(int i=0 ; i < particles.size(); ++i)
        particle_velocities[i] +=
                Vector2(cursoru.lerp(particles[i]), cursorv.lerp(particles[i]));
    lastCursorPos=vec;
    lastCursorPoll=time;

}
