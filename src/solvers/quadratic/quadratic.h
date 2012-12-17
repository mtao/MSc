#ifndef QUADRATIC_H
#define QUADRATIC_H
#include "geometry/util/types.h"

/*
0 = Mz+q
min_z  z'Mz + q'z + c
  */
template <typename Matrix, typename Vector>
struct SolverCapsule
{
    SolverCapsule(const Matrix & M_, const Vector & q_, Vector & z_): M(M_), q(q_), z(z_) {}
    typedef typename Vector::Scalar Scalar;
    virtual void step() = 0;
    virtual Scalar error()
    {
        w = M*z-q;
        //std::cout << w.cwiseProduct(z).template lpNorm<Eigen::Infinity>() << " " << w.array().min(Scalar(0)).sum() << " " << z.array().min(Scalar(0)).sum() << std::endl;
        return w.cwiseProduct(z).template lpNorm<Eigen::Infinity>()
                - w.array().min(Scalar(0)).sum()
                - z.array().min(Scalar(0)).sum()
                ;
    }
    virtual void setTolerance(const Scalar eps){epsilon = eps;}

protected:
    const Matrix & M;
    const Vector & q;
    Vector w;
    Vector & z;
    Scalar epsilon;
};


template <typename DerivedCapsule, typename Matrix, typename Vector>
DerivedCapsule createCapsule(const Matrix & M, const Vector & q, Vector & z)
{
    return DerivedCapsule(M,q,z);
}



template <typename Capsule>
struct IterativeSolver
{
    typedef typename Capsule::Matrix Matrix;
    typedef typename Capsule::Vector Vector;
    typedef typename Vector::Scalar Scalar;
    IterativeSolver(uint max_its=1000, Scalar eps=0.001):
        epsilon(eps), max_iterations(max_its) {}
    void solve()
    {
        if(!capsule){return;}
        uint iterations = 0;
        while(++iterations < max_iterations && capsule->error() > epsilon) {
        //do {
            //std::cout << "<< " << capsule->error() << " " << epsilon << " " << capsule->error() << std::endl;
            capsule->step();
        }
        //while(++iterations < max_iterations && capsule->error() > epsilon);
        if(iterations >= max_iterations)
            std::cout << iterations << "/" << max_iterations << ": " << capsule->error() << "/" << epsilon << std::endl;
    }
    void solve(const Matrix & M, const Vector & q, Vector & z)
    {
        capsule.reset(new Capsule(M,q,z));
        capsule->setTolerance(epsilon);
        solve();
    }
    bool success()
    {return (epsilon >= capsule->error());}

private:
    std::unique_ptr<Capsule> capsule;
    Scalar epsilon;
    uint max_iterations;
};
#endif // QUADRATIC_H
