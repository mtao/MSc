#include "sim.h"
#include "solvers/linear/conjugate_gradient/pcg.hpp"
#include "simulation/utils/rand.h"
#include "geometry/util/bound.h"
#include "scene.h"


void MACStableFluidsFreeSurface2::compute_phi(CGrid & phi)
{
    compute_liquid_phi(phi);
    compute_solid_lphi(phi);
}
void MACStableFluidsFreeSurface2::compute_liquid_phi(CGrid & phi)
{

    uint i,j; Scalar fi,fj;
    fi = 10*m_dx;
    phi.fill(fi);
    for(std::vector<Vector2>::iterator it=particles.begin(); it<particles.end(); it++)
    {

        phi.barycentric(*it,i,fi,j,fj);
        Vector2 p;
        int extrap_size = particle_radius*phi.NI()+1;
        Scalar tmp_phi;
        for(int j_off = j-extrap_size; j_off <= j+extrap_size; ++j_off)
            for(int i_off = i-extrap_size; i_off <= i+extrap_size; ++i_off)
            {
                if(i_off<0 || i_off >= phi.NI()
                        || j_off < 0 || j_off >= phi.NJ())
                    continue;
                p(0)=(i_off+0.5)/phi.NI();
                p(1)=(j_off+0.5)/phi.NJ();
                tmp_phi = (p-*it).norm()-1.02*particle_radius;
                phi(i_off,j_off) = std::min(phi(i_off,j_off), tmp_phi);
            }
    }
}
void MACStableFluidsFreeSurface2::compute_solid_lphi(CGrid & phi)
{
    uint i,j; Scalar fi,fj;
    Scalar s_phi;
    foreach2d<CGrid>([&](int i, int j)
    {
        if(phi(i,j) < 0.5*m_dx)
        {
            s_phi = .25f*(
                        sphi(i  ,j   )
                        +sphi(i+1,j  )
                        +sphi(i  ,j+1)
                        +sphi(i+1,j+1)
                        );
            if(s_phi < 0)
                phi(i,j) = -0.5*m_dx;

        }
    });



}
void MACStableFluidsFreeSurface2::init()
{
    uw.fill(0);
    vw.fill(0);
    p.fill(0);
    u.fill(0);
    v.fill(0);
    pressure.setZero();
    initScene(m_dx,u,v,uw,vw,p,lphi,sphi,particles,particle_radius);

}
void MACStableFluidsFreeSurface2::compute_uv_weights()
{
    uint i,j;
    for(j=1; j<uw.NJ()-1; ++j)
        for(i=0; i<uw.NI(); ++i)
        {
            uw(i,j) = 1-fraction_inside(sphi(i,j),sphi(i,j+1));
            uw(i,j) = clamp(uw(i,j),0.0f,1.0f);
        }
    for(j=0; j<vw.NJ(); ++j)
        for(i=1; i<vw.NI()-1; ++i)
        {
            vw(i,j) = 1-fraction_inside(sphi(i,j),sphi(i+1,j));
            vw(i,j) = clamp(vw(i,j),0.0f,1.0f);
        }
}
void MACStableFluidsFreeSurface2::step(Scalar dt)
{
    float t=0;
    int count=0;
    while(t<dt)
    {
        float substep=cfl();

        if(substep<0.00000001)
        {
            std::cerr << "SUBSTEP GOING TOO LOW QUITTING\n";
            return;
        }
        if(t+substep > dt)
            substep=dt-t;

        advect_particles(dt);

        add_force(dt);
        advect(dt);
        diffuse(dt);
        project(dt);
        extrapolate(u,uv);
        extrapolate(v,vv);
        constrain_velocity();
        ++count;
        t+=substep;
    }
    time += dt;


}
void MACStableFluidsFreeSurface2::add_force(Scalar dt)
{
    foreach2d_ghost<VGrid>([&](int i, int j)
    {v(i,j) += -0.8*dt;});

}

void MACStableFluidsFreeSurface2::project(Scalar dt)
{

    auto & d12 = getD12();
    compute_phi(lphi);
    compute_uv_weights();
    Vector V(fluxSize());
    V.segment(matIndex<UGrid>(0,0),size<UGrid>()) = u.mapVector().cwiseProduct(uw.mapVector());
    V.segment(matIndex<VGrid>(0,0),size<VGrid>()) = v.mapVector().cwiseProduct(vw.mapVector());
    Vector rhs =  d12 * V;
    rhs = (lphi.mapVector().array()<0).select(rhs,Scalar(0));
    MACStableFluids2::rhs.mapVector() = 100*rhs;




    //p is negative
    /*
    p.mapVector() = (lphi.mapVector().array()<=Scalar(0)).select(Vector::Ones(lphi.size(),1),Scalar(0));
    */
    Vector vlphi = d12.transpose() *
            (lphi.mapVector().array()<=Scalar(0)).select(Vector::Ones(lphi.size(),1),Scalar(0));


    //check fraction inside (u flux)
    V.setZero();
    foreach2d_ghost<UGrid>([&](int i, int j)
    {
        if(lphi(i,j) < 0 && lphi(i-1,j) < 0)
            V(matIndex<UGrid>(i,j)) = uw(i,j);
    });
    foreach2d_ghost<VGrid>([&](int i, int j)
    {
        if(lphi(i,j) < 0 && lphi(i,j-1) < 0)
            V(matIndex<VGrid>(i,j)) = vw(i,j);
    });


    SparseMatrix L = d12 * V.asDiagonal();
    L =  (L * d12.transpose()).eval();








    vlphi.setZero();

    foreach2d_ghost<CGrid>([&](int i, int j)
    {
        uint ind = index<CGrid>(i,j);
        Scalar clphi = lphi(i,j);
        if(clphi<0)
        {


            Scalar rlphi = lphi(i+1,j);
            if(rlphi>0)
            {
                Scalar theta = fraction_inside(clphi,rlphi);
                if(theta<0.01) theta = 0.01;
                vlphi(ind) += uw(i+1,j)/theta;
            }


            Scalar llphi = lphi(i-1,j);
            if(llphi>0)
            {
                Scalar theta = fraction_inside(clphi,llphi);
                if(theta<0.01) theta = 0.01;
                vlphi(ind) += uw(i,j)/theta;
            }



            Scalar tlphi = lphi(i,j+1);
            if(tlphi>0)
            {
                Scalar theta = fraction_inside(clphi,tlphi);
                if(theta<0.01) theta = 0.01;
                vlphi(ind) += vw(i,j+1)/theta;
            }


            Scalar blphi = lphi(i,j-1);
            if(blphi>0)
            {
                Scalar theta = fraction_inside(clphi,blphi);
                if(theta<0.01) theta = 0.01;
                vlphi(ind) += vw(i,j)/theta;
            }
        }


    });


    for(int i=0; i < L.rows() ; ++i)
        L.coeffRef(i,i) += vlphi(i);


    Eigen::VectorXd rhsd = rhs.cast<double>();
    Eigen::SparseMatrix<double> Ld = L.cast<double>();
    pressure.setZero();
    SparseCholeskyPreconditionedConjugateGradientSolve(Ld,rhsd,pressure);
    p.mapVector()=pressure.cast<float>();
    Vector gradP =  getD12().transpose()*p.mapVector();
    p.mapVector()=100*pressure.cast<float>();
    uv.fill(0);
    vv.fill(0);
    foreach2d_ghost<UGrid>([&](int i, int j)
    {
        if(uw(i,j)>0.0001 && (lphi(i,j)<0 || lphi(i-1,j)<0))
        {
            Scalar theta = 1;
            if(lphi(i,j)>=0 || lphi(i-1,j)>=0)
            {
                theta = fraction_inside(lphi(i-1,j), lphi(i,j));
            }
            if(theta<0.01)
            {
                theta = 0.01;
            }
            u(i,j) -= uMapMatrix(gradP.data())(i,j)/theta;
            uv(i,j)=1;
        }
        else
        {
        }
    });
    foreach2d_ghost<VGrid>([&](int i, int j)
    {
        if(vw(i,j)>0.0001 && (lphi(i,j)<0 || lphi(i,j-1)<0))
        {
            Scalar theta = 1;
            if(lphi(i,j)>=0 || lphi(i,j-1)>=0)
            {
                theta = fraction_inside(lphi(i,j-1), lphi(i,j));
            }
            if(theta<0.01)
            {
                theta = 0.01;
            }
            v(i,j) -= vMapMatrix(gradP.data())(i,j)/theta;
            vv(i,j)=1;
        }
        else
        {
        }
    });

}

void MACStableFluidsFreeSurface2::advect_particles(Scalar dt)
{
    std::for_each(particles.begin(), particles.end(), [&](Vector2 & p)
    {
        MACFluidAdvection::advect_rk2(u,v,p,dt);
        Scalar psphi = sphi.lerp(p);
        if(psphi < 0)
        {
            Vector2 normal;
            sphi.gradient(p,normal);
            normal.normalize();
            p -= psphi * normal;

        }
    });
}
void MACStableFluidsFreeSurface2::diffuse(Scalar dt)
{
}

void MACStableFluidsFreeSurface2::constrain_velocity()
{
    compute_uv_weights();
    uint i,j;
    uold=u;
    vold=v;
    Vector2 p,vel=Vector2::Zero(), normal=Vector2::Zero();
    Scalar dx_2=Scalar(0.5)*m_dx;
    foreach2d<UGrid>([&](int i, int j)
    {
        if(uw(i,j)==0)//no solid constraint
        {
            uw.vertex(i,j,p);
            MACFluidAdvection::get_velocity(uold,vold,p,vel);
            sphi.gradient(p,normal);
            if(!normal.isZero() && normal.dot(vel) > 0)
            {
                //std::cout << "should be nonzero: "<<i << " " << j << std::endl;
                normal.normalize();
                Scalar proj = normal.dot(vel);
                if(proj < 0)
                u(i,j) -= proj*normal(0);
            }
        }
        if(i == 0 || i == NI<UGrid>()-1 || j == 0 || j == NJ<UGrid>()-1)
        {
            u(i,j) = 0;
        }
    });


    foreach2d<VGrid>([&](int i, int j)
    {
        if(vw(i,j)==0)
        {
            vw.vertex(i,j,p);
            MACFluidAdvection::get_velocity(uold,vold,p,vel);
            //std::cout<<v<<std::endl;
            sphi.gradient(p,normal);
            if(!normal.isZero())
            {
                normal.normalize();
                normal.normalize();
                Scalar proj = normal.dot(vel);
                if(proj < 0)
                v(i,j) -= proj*normal(1);
            }
        }
        if(i == 0 || i == NI<VGrid>()-1 || j == 0 || j == NJ<VGrid>()-1)
        {
            v(i,j) = 0;
        }
    });
}





