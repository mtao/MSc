#include "sim.h"
#include "particledensitycorrector.hpp"

void MACFLIP::init()
{
    MACStableFluidsFreeSurface::init();
    particle_velocities.resize(particles.size(), Vector3(0,0,0));
}


void MACFLIP::add_force(Scalar dt)
{
    MACStableFluidsFreeSurface::add_force(dt);
    std::for_each(particle_velocities.begin(), particle_velocities.end(), [&](Vector3 & p)
    {p(1) += -0.8*dt;});

}
void MACFLIP::advect_particles(Scalar dt)
{
    for(int i=0; i < u.size(); ++i) pu(i) = u(i) - pu(i);
    for(int i=0; i < v.size(); ++i) pv(i) = v(i) - pv(i);
    for(int i=0; i < w.size(); ++i) pw(i) = w(i) - pw(i);
    for(int i=0; i<particles.size(); ++i)
    {

        particle_velocities[i] +=
                Vector3(pu.lerp(particles[i]), pv.lerp(particles[i]),pw.lerp(particles[i]));
        MACFluidAdvection::advect_rk2(u,v,w,particles[i],dt);
        //particles[i] += particle_velocities[i]*dt; //A terrible move

        Scalar psphi = sphi.lerp(particles[i])+particle_radius;
        if(psphi < 0)
        {
            Vector3 normal;
            sphi.gradient(particles[i],normal);
            normal.normalize();
            particle_velocities[i] -= particle_velocities[i].dot(normal) * normal;
            particles[i] -= psphi * normal;

        }
    }
}

auto MACFLIP::cfl() -> decltype(m_dx)
{
    return m_dx/std::max(std::max(
                u.mapVector().lpNorm<Eigen::Infinity>(),
                v.mapVector().lpNorm<Eigen::Infinity>()),
                w.mapVector().lpNorm<Eigen::Infinity>()
                );
}

void MACFLIP::step(Scalar dt)
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
        pw = w;

        //standard sim stuff
        diffuse(dt);
        project(dt);//uw and vw are set here
        extrapolate(u,uv);
        extrapolate(v,vv);
        extrapolate(w,wv);
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
auto MACFLIP::Weight(Scalar x, Scalar y, Scalar z) -> Scalar
{
    return std::max(
                Scalar(0),
                    std::pow(
                        1
                    -(x*x+y*y+z*z),Scalar(3)
                        )
                    );
}

void MACFLIP::advect(Scalar dt)
{
    uw.fill(0);
    vw.fill(0);
    ww.fill(0);
    u.fill(0);
    v.fill(0);
    w.fill(0);
    Scalar fi,fj,fk;
    uint i,j,k;
    for(int m=0; m<particles.size(); ++m)
    {
        const Vector3 & p = particles[m];
        const Vector3 & vel = particle_velocities[m];

        u.barycentric(p,i,fi,j,fj,k,fk);

        add_weight(u,uw,vel,i,fi,j,fj,k,fk,0,0,0,0);
        add_weight(u,uw,vel,i,fi,j,fj,k,fk,0,0,1,0);
        add_weight(u,uw,vel,i,fi,j,fj,k,fk,0,1,0,0);
        add_weight(u,uw,vel,i,fi,j,fj,k,fk,0,1,1,0);
        add_weight(u,uw,vel,i,fi,j,fj,k,fk,1,0,0,0);
        add_weight(u,uw,vel,i,fi,j,fj,k,fk,1,0,1,0);
        add_weight(u,uw,vel,i,fi,j,fj,k,fk,1,1,0,0);
        add_weight(u,uw,vel,i,fi,j,fj,k,fk,1,1,1,0);

        v.barycentric(p,i,fi,j,fj,k,fk);
        add_weight(v,vw,vel,i,fi,j,fj,k,fk,0,0,0,1);
        add_weight(v,vw,vel,i,fi,j,fj,k,fk,0,0,1,1);
        add_weight(v,vw,vel,i,fi,j,fj,k,fk,0,1,0,1);
        add_weight(v,vw,vel,i,fi,j,fj,k,fk,0,1,1,1);
        add_weight(v,vw,vel,i,fi,j,fj,k,fk,1,0,0,1);
        add_weight(v,vw,vel,i,fi,j,fj,k,fk,1,0,1,1);
        add_weight(v,vw,vel,i,fi,j,fj,k,fk,1,1,0,1);
        add_weight(v,vw,vel,i,fi,j,fj,k,fk,1,1,1,1);


        w.barycentric(p,i,fi,j,fj,k,fk);
        add_weight(w,ww,vel,i,fi,j,fj,k,fk,0,0,0,2);
        add_weight(w,ww,vel,i,fi,j,fj,k,fk,0,0,1,2);
        add_weight(w,ww,vel,i,fi,j,fj,k,fk,0,1,0,2);
        add_weight(w,ww,vel,i,fi,j,fj,k,fk,0,1,1,2);
        add_weight(w,ww,vel,i,fi,j,fj,k,fk,1,0,0,2);
        add_weight(w,ww,vel,i,fi,j,fj,k,fk,1,0,1,2);
        add_weight(w,ww,vel,i,fi,j,fj,k,fk,1,1,0,2);
        add_weight(w,ww,vel,i,fi,j,fj,k,fk,1,1,1,2);
    }
    foreach3d_ghost<UGrid>([&](int i, int j,int k)
    {
        if(uw(i,j,k)>0)
            u(i,j,k) /= uw(i,j,k);
    });
    foreach3d_ghost<VGrid>([&](int i, int j,int k)
    {
        if(vw(i,j,k)>0)
            v(i,j,k) /= vw(i,j,k);
    });
    foreach3d_ghost<WGrid>([&](int i, int j,int k)
    {
        if(ww(i,j,k)>0)
            w(i,j,k) /= ww(i,j,k);
    });
    foreach_boundary<UGrid>([&](int i,int j, int k)
    {
        u(i,j,k) = 0;
    });
    foreach_boundary<VGrid>([&](int i,int j,int k)
    {
        v(i,j,k) = 0;
    });
    foreach_boundary<WGrid>([&](int i,int j,int k)
    {
        w(i,j,k) = 0;
    });
}

void MACFLIP::cursorPos(Vector2 vec)
{

}
