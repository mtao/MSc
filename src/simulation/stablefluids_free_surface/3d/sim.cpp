#include "sim.h"
#include "solvers/linear/conjugate_gradient/pcg.hpp"
#include "simulation/utils/rand.h"
#include "geometry/util/bound.h"
#include "scene.h"


namespace batty{
float fraction_inside(float phi_left, float phi_right) {
   if(phi_left < 0 && phi_right < 0)
      return 1;
   if (phi_left < 0 && phi_right >= 0)
      return phi_left / (phi_left - phi_right);
   if(phi_left >= 0 && phi_right < 0)
      return phi_right / (phi_right - phi_left);
   else
      return 0;
}

static void cycle_array(float* arr, int size) {
   float t = arr[0];
   for(int i = 0; i < size-1; ++i)
      arr[i] = arr[i+1];
   arr[size-1] = t;
}

//Given four signed distance values (square corners), determine what fraction of the square is "inside"
float fraction_inside(float phi_bl, float phi_br, float phi_tl, float phi_tr) {

   int inside_count = (phi_bl<0?1:0) + (phi_tl<0?1:0) + (phi_br<0?1:0) + (phi_tr<0?1:0);
   float list[] = { phi_bl, phi_br, phi_tr, phi_tl };

   if(inside_count == 4)
      return 1;
   else if (inside_count == 3) {
      //rotate until the positive value is in the first position
      while(list[0] < 0) {
         cycle_array(list,4);
      }

      //Work out the area of the exterior triangle
      float side0 = 1-fraction_inside(list[0], list[3]);
      float side1 = 1-fraction_inside(list[0], list[1]);
      return 1 - 0.5f*side0*side1;
   }
   else if(inside_count == 2) {

      //rotate until a negative value is in the first position, and the next negative is in either slot 1 or 2.
      while(list[0] >= 0 || !(list[1] < 0 || list[2] < 0)) {
         cycle_array(list,4);
      }

      if(list[1] < 0) { //the matching signs are adjacent
         float side_left = fraction_inside(list[0], list[3]);
         float side_right = fraction_inside(list[1], list[2]);
         return  0.5f*(side_left + side_right);
      }
      else { //matching signs are diagonally opposite
         //determine the centre point's sign to disambiguate this case
         float middle_point = 0.25f*(list[0] + list[1] + list[2] + list[3]);
         if(middle_point < 0) {
            float area = 0;

            //first triangle (top left)
            float side1 = 1-fraction_inside(list[0], list[3]);
            float side3 = 1-fraction_inside(list[2], list[3]);

            area += 0.5f*side1*side3;

            //second triangle (top right)
            float side2 = 1-fraction_inside(list[2], list[1]);
            float side0 = 1-fraction_inside(list[0], list[1]);
            area += 0.5f*side0*side2;

            return 1-area;
         }
         else {
            float area = 0;

            //first triangle (bottom left)
            float side0 = fraction_inside(list[0], list[1]);
            float side1 = fraction_inside(list[0], list[3]);
            area += 0.5f*side0*side1;

            //second triangle (top right)
            float side2 = fraction_inside(list[2], list[1]);
            float side3 = fraction_inside(list[2], list[3]);
            area += 0.5f*side2*side3;
            return area;
         }

      }
   }
   else if(inside_count == 1) {
      //rotate until the negative value is in the first position
      while(list[0] >= 0) {
         cycle_array(list,4);
      }

      //Work out the area of the interior triangle, and subtract from 1.
      float side0 = fraction_inside(list[0], list[3]);
      float side1 = fraction_inside(list[0], list[1]);
      return 0.5f*side0*side1;
   }
   else
      return 0;

}

};
#include <algorithm>

void MACStableFluidsFreeSurface::compute_phi(CGrid & phi)
{
    compute_liquid_phi(phi);
    compute_solid_lphi(phi);
}
void MACStableFluidsFreeSurface::compute_liquid_phi(CGrid & phi)
{

    uint i,j,k; Scalar fi,fj,fk;
    fi = 10*m_dx;
    phi.fill(fi);
    for(auto it=particles.begin(); it<particles.end(); it++)
    {

        phi.barycentric(*it,i,fi,j,fj,k,fk);
        Vector3 p;
        int extrap_size = particle_radius*phi.NI()+1;
        Scalar tmp_phi;
        for(int k_off = k-extrap_size; k_off <= k+extrap_size; ++k_off)
        for(int j_off = j-extrap_size; j_off <= j+extrap_size; ++j_off)
            for(int i_off = i-extrap_size; i_off <= i+extrap_size; ++i_off)
            {
                if(
                        i_off<0 || i_off >= phi.NI()
                        || j_off < 0 || j_off >= phi.NJ()
                        || k_off < 0 || k_off >= phi.NK()
                        )
                    continue;
                p=phi.vertex(i_off,j_off,k_off);
                tmp_phi = (p-*it).norm()-1.02*particle_radius;
                phi(i_off,j_off,k_off) = std::min(phi(i_off,j_off, k_off), tmp_phi);

            }
    }
}
void MACStableFluidsFreeSurface::compute_solid_lphi(CGrid & phi)
{
    uint i,j,k; Scalar fi,fj,fk;
    Scalar s_phi;
    foreach3d<CGrid>([&](int i, int j,int k)
    {
        if(phi(i,j,k) < 0.5*m_dx)
        {
            s_phi = .125f*(
                        sphi(i  ,j   ,k  )
                        +sphi(i+1,j  ,k  )
                        +sphi(i  ,j+1,k  )
                        +sphi(i+1,j+1,k  )
                        +sphi(i  ,j  ,k+1)
                        +sphi(i+1,j  ,k+1)
                        +sphi(i  ,j+1,k+1)
                        +sphi(i+1,j+1,k+1)
                        );
            if(s_phi < 0)
                phi(i,j,k) = -0.5*m_dx;

        }
    });



}
template <typename Grid>
typename Grid::Scalar center(Grid & g,int i, int j, int k)
{
            return .125*(
                        g(i  ,j   ,k  )
                        +g(i+1,j  ,k  )
                        +g(i  ,j+1,k  )
                        +g(i+1,j+1,k  )
                        +g(i  ,j  ,k+1)
                        +g(i+1,j  ,k+1)
                        +g(i  ,j+1,k+1)
                        +g(i+1,j+1,k+1)
                        );
}
void MACStableFluidsFreeSurface::init()
{
    uw.fill(0);
    vw.fill(0);
    ww.fill(0);
    p.fill(0);
    u.fill(0);
    v.fill(0);
    w.fill(0);
    initScene(m_dx,u,v,w,uw,vw,ww,p,lphi,sphi,particles,particle_radius);

}
void MACStableFluidsFreeSurface::compute_uvw_weights()
{
    foreach3d_ghost<UGrid>([&](int i, int j, int k)
    {
            uw(i,j,k) = 1-batty::fraction_inside(sphi(i,j,k),sphi(i,j+1,k),sphi(i,j,k+1),sphi(i,j+1,k+1));
            uw(i,j,k) = clamp(uw(i,j,k),0.0f,1.0f);
            return;
            std::cout <<
            sphi(i,j,k) << " " <<
                    sphi(i,j+1,k) << " " <<
                    sphi(i,j,k+1) << " " <<
                    sphi(i,j+1,k+1) << ": " << uw(i,j,k) << std::endl;
    });
    foreach3d_ghost<VGrid>([&](int i, int j, int k)
    {
            vw(i,j,k) = 1-batty::fraction_inside(sphi(i,j,k),sphi(i+1,j,k),sphi(i,j,k+1),sphi(i+1,j,k+1));
            vw(i,j,k) = clamp(vw(i,j,k),0.0f,1.0f);
        });
    foreach3d_ghost<WGrid>([&](int i, int j, int k)
    {
            ww(i,j,k) = 1-batty::fraction_inside(sphi(i,j,k),sphi(i+1,j,k),sphi(i,j+1,k),sphi(i+1,j+1,k));
            ww(i,j,k) = clamp(ww(i,j,k),0.0f,1.0f);
        });
}
void MACStableFluidsFreeSurface::step(Scalar dt)
{
    Scalar t=0;
    int count=0;
    while(t<dt)
    {
        Scalar substep=cfl();
        /*
    std::cout <<
                u.mapVector().lpNorm<Eigen::Infinity>() << " ";
    std::cout <<
                v.mapVector().lpNorm<Eigen::Infinity>() << " " ;
    std::cout <<
                w.mapVector().lpNorm<Eigen::Infinity>() << std::endl;
    */

        if(substep<0.00001)
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
        extrapolate(w,wv);
        constrain_velocity();
        ++count;
        t+=substep;
    }
    time += dt;


}
void MACStableFluidsFreeSurface::add_force(Scalar dt)
{
    foreach3d_ghost<VGrid>([&](int i, int j,int k)
    {v(i,j,k) += -0.8*dt;});

}

void MACStableFluidsFreeSurface::project(Scalar dt)
{
    compute_phi(lphi);
    compute_uvw_weights();

    auto & d23 = getD23();
    Vector V(fluxSize());
    V.segment(matIndex<UGrid>(0,0,0),size<UGrid>()) = u.mapVector().cwiseProduct(uw.mapVector());
    V.segment(matIndex<VGrid>(0,0,0),size<VGrid>()) = v.mapVector().cwiseProduct(vw.mapVector());
    V.segment(matIndex<WGrid>(0,0,0),size<WGrid>()) = w.mapVector().cwiseProduct(ww.mapVector());
    Vector rhs =  d23 * V;
    rhs = (lphi.mapVector().array()<0).select(rhs,Scalar(0));
    MACStableFluids::rhs.mapVector() = 100*rhs;




    //p is negative
    Vector vlphi = d23.transpose() *
            (lphi.mapVector().array()<=Scalar(0)).select(Vector::Ones(lphi.size(),1),Scalar(0));


    //check fraction inside (u flux)
    V.setZero();
    foreach3d_ghost<UGrid>([&](int i, int j, int k)
    {
        if(lphi(i,j,k) < 0 && lphi(i-1,j,k) < 0)
            V(matIndex<UGrid>(i,j,k)) = uw(i,j,k);
    });
    foreach3d_ghost<VGrid>([&](int i, int j, int k)
    {
        if(lphi(i,j,k) < 0 && lphi(i,j-1,k) < 0)
            V(matIndex<VGrid>(i,j,k)) = vw(i,j,k);
    });
    foreach3d_ghost<WGrid>([&](int i, int j, int k)
    {
        if(lphi(i,j,k) < 0 && lphi(i,j,k-1) < 0)
            V(matIndex<WGrid>(i,j,k)) = ww(i,j,k);
    });




    SparseMatrix L = d23 * V.asDiagonal();
    L =  (L * d23.transpose()).eval();








    vlphi.setZero();

    foreach3d_ghost<CGrid>([&](int i, int j, int k)
    {
        uint ind = index<CGrid>(i,j,k);
        Scalar clphi = lphi(i,j,k);
        if(clphi<0)
        {


            Scalar rlphi = lphi(i+1,j,k);
            if(rlphi>0)
            {
                Scalar theta = batty::fraction_inside(clphi,rlphi);
                if(theta<0.01) theta = 0.01;
                vlphi(ind) += uw(i+1,j,k)/theta;
            }


            Scalar llphi = lphi(i-1,j,k);
            if(llphi>0)
            {
                Scalar theta = batty::fraction_inside(clphi,llphi);
                if(theta<0.01) theta = 0.01;
                vlphi(ind) += uw(i,j,k)/theta;
            }



            Scalar tlphi = lphi(i,j+1,k);
            if(tlphi>0)
            {
                Scalar theta = batty::fraction_inside(clphi,tlphi);
                if(theta<0.01) theta = 0.01;
                vlphi(ind) += vw(i,j+1,k)/theta;
            }


            Scalar blphi = lphi(i,j-1,k);
            if(blphi>0)
            {
                Scalar theta = batty::fraction_inside(clphi,blphi);
                if(theta<0.01) theta = 0.01;
                vlphi(ind) += vw(i,j,k)/theta;
            }

            Scalar nlphi = lphi(i,j,k-1);
            if(nlphi>0)
            {
                Scalar theta = batty::fraction_inside(clphi,nlphi);
                if(theta<0.01) theta = 0.01;
                vlphi(ind) += ww(i,j,k)/theta;
            }


            Scalar flphi = lphi(i,j,k+1);
            if(flphi>0)
            {
                Scalar theta = batty::fraction_inside(clphi,flphi);
                if(theta<0.01) theta = 0.01;
                vlphi(ind) += vw(i,j,k+1)/theta;
            }
        }


    });

    for(int i=0; i < L.rows() ; ++i)
        L.coeffRef(i,i) += vlphi(i);
    Eigen::VectorXd pressure(rhs.rows());
    Eigen::VectorXd rhsd = rhs.cast<double>();
    Eigen::SparseMatrix<double> Ld = L.cast<double>();
    pressure.setZero();
    SparseCholeskyPreconditionedConjugateGradientSolve(Ld,rhsd,pressure);
    p.mapVector()=pressure.cast<Scalar>();
    Vector gradP =  getD23().transpose()*p.mapVector();
    p.mapVector()=100*pressure.cast<Scalar>();
    uv.fill(0);
    vv.fill(0);
    wv.fill(0);
    foreach3d_ghost<UGrid>([&](int i, int j, int k)
    {
        if(uw(i,j,k)>0.0001 && (lphi(i,j,k)<0 || lphi(i-1,j,k)<0))
        {
            Scalar theta = 1;
            if(lphi(i,j,k)>=0 || lphi(i-1,j,k)>=0)
            {
                theta = batty::fraction_inside(lphi(i-1,j,k), lphi(i,j,k));
            }
            if(theta<0.01)
            {
                theta = 0.01;
            }
            u(i,j,k) -= gradP(matIndex<UGrid>(i,j,k))/theta;
            uv(i,j,k) = 1;
        }
        else
        {
        }
    });
    foreach3d_ghost<VGrid>([&](int i, int j, int k)
    {
        if(vw(i,j,k)> 0.0001 && (lphi(i,j,k)<0 || lphi(i,j-1,k)<0))
        {
            Scalar theta = 1;
            if(lphi(i,j,k)>=0 || lphi(i,j-1,k)>=0)
            {
                theta = batty::fraction_inside(lphi(i,j-1,k), lphi(i,j,k));
            }
            if(theta<0.01)
            {
                theta = 0.01;
            }
            v(i,j,k) -= gradP(matIndex<VGrid>(i,j,k))/theta;
            vv(i,j,k)=1;
        }
        else
        {
        }
    });
    foreach3d_ghost<WGrid>([&](int i, int j, int k)
    {
        if(ww(i,j,k)>0.0001 && (lphi(i,j,k)<0 || lphi(i,j,k-1)<0))
        {
            Scalar theta = 1;
            if(lphi(i,j,k)>=0 || lphi(i,j,k-1)>=0)
            {
                theta = batty::fraction_inside(lphi(i,j,k-1), lphi(i,j,k));
            }
            if(theta<0.01)
            {
                theta = 0.01;
            }
            w(i,j,k) -= gradP(matIndex<WGrid>(i,j,k))/theta;
            wv(i,j,k)=1;
        }
        else
        {
        }
    });










































}

void MACStableFluidsFreeSurface::advect_particles(Scalar dt)
{
    std::for_each(particles.begin(), particles.end(), [&](Vector3 & p)
    {
        MACFluidAdvection::advect_rk2(u,v,w,p,dt);
        Scalar psphi = sphi.lerp(p);
        if(psphi < 0)
        {
            Vector3 normal;
            sphi.gradient(p,normal);
            normal.normalize();
            p -= psphi * normal;

        }
    });
}
void MACStableFluidsFreeSurface::diffuse(Scalar dt)
{
}

void MACStableFluidsFreeSurface::constrain_velocity()
{
    uint i,j,k;
    uold=u;
    vold=v;
    wold=w;
    Vector3 p,vel=Vector3::Zero(), normal=Vector3::Zero();
    Scalar dx_2=Scalar(0.5)*m_dx;
    foreach3d<UGrid>([&](int i, int j, int k)
    {
        if(uw(i,j,k)==0)//no solid constraint
        {
            uw.vertex(i,j,k,p);
            MACFluidAdvection::get_velocity(uold,vold,wold,p,vel);
            sphi.gradient(p,normal);
            if(!normal.isZero() && normal.dot(vel) > 0)
            {
                normal.normalize();
                Scalar proj = normal.dot(vel);
                if(proj < 0)
                u(i,j,k) -= proj*normal(0);
            }
        }
        if(i == 0 || i == NI<UGrid>()-1
                || j == 0 || j == NJ<UGrid>()-1
                || k == 0 || j == NK<UGrid>()-1
                )
        {
            u(i,j,k) = 0;
        }
    });


    foreach3d<VGrid>([&](int i, int j, int k)
    {
        if(vw(i,j,k)==0)
        {
            vw.vertex(i,j,k,p);
            MACFluidAdvection::get_velocity(uold,vold,wold,p,vel);
            sphi.gradient(p,normal);
            if(!normal.isZero())
            {
                normal.normalize();
                Scalar proj = normal.dot(vel);
                if(proj < 0)
                v(i,j,k) -= proj*normal(1);
            }
        }
        if(i == 0 || i == NJ<VGrid>()-1
                || j == 0 || j == NJ<VGrid>()-1
                || k == 0 || j == NK<VGrid>()-1
                )
        {
            v(i,j,k) = 0;
        }
    });
    foreach3d<WGrid>([&](int i, int j, int k)
    {
        if(ww(i,j,k)==0)
        {
            ww.vertex(i,j,k,p);
            MACFluidAdvection::get_velocity(uold,vold,wold,p,vel);
            sphi.gradient(p,normal);
            if(!normal.isZero())
            {
                normal.normalize();
                Scalar proj = normal.dot(vel);
                if(proj < 0)
                w(i,j,k) -= proj*normal(2);
            }
        }
        if(i == 0 || i == NJ<WGrid>()-1
                || j == 0 || j == NJ<WGrid>()-1
                || k == 0 || j == NK<WGrid>()-1
                )
        {
            w(i,j,k) = 0;
        }
    });
}





