#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include "solvers/quadratic/gauss_seidel/gauss_seidel.hpp"
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
    x = x.array().array();
    b = A * x;
    auto sln(x);
    x = decltype(x)::Zero(x.rows(),x.cols());
    DenseGaussSeidelSubspaceMinimizationSolve(A,b,x);
    std::cout << "Dense Test " << dim << ": lpnorm<infty>=" << (x-sln).lpNorm<Eigen::Infinity>()/x.lpNorm<Eigen::Infinity>() << std::endl;



}

void run_sparse_test(int dim)
{
    MAC2f mac(dim,dim);
    Eigen::SparseMatrix<float> A = mac.getD12() * mac.getD12().transpose();
    Eigen::VectorXf x = decltype(x)::Random(A.cols());
    Eigen::VectorXf b(A.rows());
    x = x.array().abs();
    b = A * x;
    auto sln(x);
    x = decltype(x)::Zero(x.rows(),x.cols());
    SparseGaussSeidelSubspaceMinimizationSolve(A,b,x);
    std::cout << "Sparse Test " << dim << ": lpnorm<infty>=" << (x-sln).lpNorm<Eigen::Infinity>()/x.lpNorm<Eigen::Infinity>() << std::endl;
}

int main()
{
    int dims[] = {5, 10, 50, 100, 500};
    for(int & dim: dims)
    {
        //run_dense_test(dim);
    }
    for(int & dim: dims)
    {
        run_sparse_test(dim);
    }
}

