#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include "solvers/linear/conjugate_gradient/cg.hpp"
#include "solvers/linear/conjugate_gradient/pcg.hpp"
#include <vector>
#include "geometry/grid/mac/mac.h"


void run_dense_test(int dim)
{
    Eigen::MatrixXf A = decltype(A)::Random(dim,dim);
    Eigen::VectorXf x = decltype(x)::Random(dim);
    Eigen::VectorXf b(dim);
    A = (A * A.transpose()).eval();
    const float eps = 0.0001;
    for(int i = 0; i < A.rows(); ++i)
    {
        if(std::abs(A(i,i))<eps)
        {
            A(i,i) = eps;
        }
        A(i,i) = A.row(i).lpNorm<1>();
    }
    b = A * x;
    auto sln(x);
    x = decltype(x)::Zero(x.rows(),x.cols());
    ConjugateGradientSolve(A,b,x);
    std::cout << "Dense Test " << dim << ": lpnorm<infty>=" << (x-sln).lpNorm<Eigen::Infinity>() << std::endl;



}

void run_sparse_test(int dim)
{
    MAC2f mac(dim,dim);
    Eigen::SparseMatrix<float> A = mac.getD12() * mac.getD12().transpose();
    for(int i=0 ;  i < A.rows(); ++i)
        A.coeffRef(i,i) += 1;
    Eigen::VectorXf x = decltype(x)::Random(A.cols());
    Eigen::VectorXf b(A.rows());
    b = A * x;
    auto sln(x);
    x = decltype(x)::Zero(x.rows(),x.cols());
    SparseCholeskyPreconditionedConjugateGradientSolve(A,b,x);
    std::cout << "Sparse Test " << x.rows() << ": lpnorm<infty>=" << (x-sln).lpNorm<Eigen::Infinity>() << std::endl;
}


int main()
{
    int dims[] = {5, 10, 100, 1000};
    for(int & dim: dims)
    {
        run_dense_test(dim);
    }
    for(int & dim: dims)
    {
        run_sparse_test(dim);
    }
}

