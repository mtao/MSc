#ifndef SPARSE_GAUSS_SEIDEL_H
#define SPARSE_GAUSS_SEIDEL_H

#include "gauss_seidel.hpp"

typedef Solver<GaussSeidelEngine<float,matrix_tags::sparse_matrix , Eigen::Dynamic> > SparseSolverGaussSeidelXf;
typedef Solver<GaussSeidelEngine<double,matrix_tags::sparse_matrix, Eigen::Dynamic> > SparseSolverGaussSeidelXd;
#endif
