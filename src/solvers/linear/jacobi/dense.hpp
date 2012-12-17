#ifndef JACOBI_DENSE_H
#define JACOBI_DENSE_H

#include "jacobi.hpp"

typedef Solver<JacobiEngine<float,matrix_tags::dense_matrix , Eigen::Dynamic > > DenseSolverJacobiXf;
typedef Solver<JacobiEngine<double,matrix_tags::dense_matrix, Eigen::Dynamic > > DenseSolverJacobiXd;

#endif
