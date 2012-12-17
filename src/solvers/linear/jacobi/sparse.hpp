#ifndef JACOBI_SPARSE_H
#define JACOBI_SPARSE_H

#include "jacobi.hpp"

typedef Solver<JacobiEngine<float,matrix_tags::sparse_matrix , Eigen::Dynamic > > SparseSolverJacobiXf;
typedef Solver<JacobiEngine<double,matrix_tags::sparse_matrix, Eigen::Dynamic > > SparseSolverJacobiXd;

#endif
