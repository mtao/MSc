#ifndef GAUSS_SEIDEL_DENSE_H
#define GAUSS_SEIDEL_DENSE_H

#include "gauss_seidel.hpp"

typedef Solver<GaussSeidelEngine<float,matrix_tags::dense_matrix , Eigen::Dynamic > > DenseSolverGaussSeidelXf;
typedef Solver<GaussSeidelEngine<double,matrix_tags::dense_matrix, Eigen::Dynamic > > DenseSolverGaussSeidelXd;

#endif
