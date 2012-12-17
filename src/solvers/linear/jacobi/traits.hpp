#ifndef JACOBI_TRAITS_H
#define JACOBI_TRAITS_H
template <typename  T, int DIM>
struct SolverEngineTraits<JacobiEngine<T, matrix_tags::dense_matrix,DIM> >
{
    typedef JacobiEngine<T, matrix_tags::dense_matrix,DIM> SolverEngine;
    typedef NumericalTraits<T,DIM> NumTraits;
    typedef typename NumTraits::Matrix Matrix;
    typedef DenseAccessor<NumTraits> Accessor;
    typedef ABXSolver<SolverEngine> SolveTraits;
    constexpr static T tolerance = 1e-5;
};


template <typename T, int DIM>
struct SolverEngineTraits<JacobiEngine<T, matrix_tags::sparse_matrix,DIM> >
{
    typedef JacobiEngine<T, matrix_tags::sparse_matrix,DIM> SolverEngine;
    typedef NumericalTraits<T,DIM> NumTraits;
    typedef typename NumTraits::SparseMatrix Matrix;
    typedef SparseAccessor<NumTraits> Accessor;
    typedef ABXSolver<SolverEngine> SolveTraits;
    constexpr static T tolerance = 1e-5;
};
#endif
