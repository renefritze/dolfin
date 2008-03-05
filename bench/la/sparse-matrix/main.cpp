// Copyright (C) 2006 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2006-08-18
// Last changed:
//
// Benchmarks for uBlas matrices

#include <dolfin.h>
#include <boost/tuple/tuple.hpp>
#include "Poisson.h"
#include "VectorPoisson.h"

using namespace dolfin;
using namespace boost::tuples;

//-----------------------------------------------------------------------------
template<class Mat, class Vec = uBlasVector>
struct MatrixAssemble
{
  //---------------------------------------------------------------------------
  static real assemble(BilinearForm& a, const dolfin::uint N)
  {
    set("output destination", "silent");  
    UnitSquare mesh(N, N);
    Mat A;
  
    tic();
    FEM::assemble(a, A, mesh);
    return toc();
  }
  //---------------------------------------------------------------------------
  static real assemble(BilinearForm& a, Mat& A, const dolfin::uint N)
  {
    set("output destination", "silent");  
    UnitSquare mesh(N, N);

    tic();
    FEM::assemble(a, A, mesh);
    return toc();
  }
  //---------------------------------------------------------------------------
  static tuple<real, real> assemble(const dolfin::uint N)
  {
    set("output destination", "silent");
    tuple<real, real> timing;
    Mat A;
    A.init(2*N, 2*N, 6);
    A.zero();
    
    real block[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}; 
    int ipos[4] = {0, 0, 0, 0};
    int jpos[4] = {0, 0, 0, 0};

    tic();
    for(dolfin::uint i = 0; i < N-1; ++i)
    {
      ipos[0] = i;   ipos[1] = i+1; ipos[2] = N + i; ipos[3] = N+i+1;
      jpos[0] = i+1; jpos[1] = i;   jpos[2] = N+i+1; jpos[3] = N+i;
      A.add(block, ipos, 4, jpos, 4);
    }
//    get<0>(timing) = toc();  
    timing.get<0>() = toc();  
  
    tic();
    A.apply();
    get<1>(timing) = toc();  

    return timing;    
  }
  //---------------------------------------------------------------------------
  static real vector_multiply(const dolfin::uint N, const dolfin::uint n)
  {
    set("output destination", "silent");
    VectorPoisson::BilinearForm a;
    Mat A;
    Vec x, y;
    UnitSquare mesh(N,N);  

    FEM::assemble(a, A, mesh); 
    x.init(A.size(1));
    y.init(A.size(1));

    tic();
    for(dolfin::uint i = 0; i < n; ++i)
      A.mult(x, y); 
    return toc();
  }
};
//-----------------------------------------------------------------------------
void AssemblePoissonMatrix()
{
  // Assembly of sparse matrices on a N x N unit mesh
  real time;
  const dolfin::uint n = 3;
  const dolfin::uint N[n] = {50, 100, 400};
  Poisson::BilinearForm a;
  VectorPoisson::BilinearForm a_vector;

  begin("Assemble a sparse matrix for scalar Poisson equation on an square N x N mesh");
#ifdef HAS_PETSC  
  for(dolfin::uint i =0; i < n; ++i)
  {
    time = MatrixAssemble< PETScMatrix >::assemble(a, N[i]);
    set("output destination", "terminal");
    cout << "PETScMatrix       (N=" << N[i] << "): " << time << endl;
  }
#endif
  for(dolfin::uint i =0; i < n; ++i)
  {
    time = MatrixAssemble< uBlasMatrix<ublas_sparse_matrix> >::assemble(a, N[i]);
    set("output destination", "terminal");
    cout << "uBlasSparseMatrix (N=" << N[i] << "): " << time << endl;
  }
  end();

  begin("Assemble a sparse matrix for vector Poisson equation on an square N x N mesh");
#ifdef HAS_PETSC  
  for(dolfin::uint i =0; i < n; ++i)
  {
    time = MatrixAssemble< PETScMatrix >::assemble(a_vector, N[i]);
    set("output destination", "terminal");
    cout << "PETScMatrix       (N=" << N[i] << "): " << time << endl;
  }
#endif
  for(dolfin::uint i =0; i < n; ++i)
  {
    time = MatrixAssemble< uBlasMatrix<ublas_sparse_matrix> >::assemble(a_vector, N[i]);
    set("output destination", "terminal");
    cout << "uBlasSparseMatrix (N=" << N[i] << "): " << time << endl;
  }
  end();
}
//-----------------------------------------------------------------------------
void AssembleSparseMatrices()
{
  // Assemble of sparse matrices of size 2*N x 2*N
  const dolfin::uint N = 500000;
  tuple<real, real> timing;

  set("output destination", "terminal");
  begin("Assemble a sparse matrix in quasi-random order (size = 2N x 2N)" );
#ifdef HAS_PETSC  
  timing = MatrixAssemble< PETScMatrix >::assemble(N);
  set("output destination", "terminal");
  cout << "PETScMatrix insert         (N=" << N << "): " << get<0>(timing) << endl;
  cout << "PETScMatrix finalise       (N=" << N << "): " << get<1>(timing) << endl;
#endif
  timing = MatrixAssemble< uBlasMatrix<ublas_sparse_matrix> >::assemble(N);
  set("output destination", "terminal");
  cout << "uBlasSparseMatrix insert   (N=" << N << "): " << get<0>(timing) << endl;
  cout << "uBlasSparseMatrix finalise (N=" << N << "): " << get<01>(timing) << endl;
        
  end();
}
//-----------------------------------------------------------------------------
void MatrixVectorMultiply()
{
  // Assembly of sparse matrices on a N x N unit mesh
  const dolfin::uint n = 200;
  const dolfin::uint N = 200;
  real time;

  set("output destination", "terminal");
  begin("Sparse matrix-vector multiplication (size N x N, repeated n times)");
#ifdef HAS_PETSC 
  time = MatrixAssemble< PETScMatrix, PETScVector >::vector_multiply(N, n);
  set("output destination", "terminal");
  cout << "PETScMatrix: (N=" << N << ", n=" << n << "): " << time << endl;
#endif
  time = MatrixAssemble< uBlasMatrix<ublas_sparse_matrix>, uBlasVector >::vector_multiply(N, n);
  set("output destination", "terminal");
  cout << "uBlasMatrix: (N=" << N << ", n=" << n << "): " << time << endl;

  end();
}
//-----------------------------------------------------------------------------
int main()
{
  begin("Sparse matrix benchmark timings");
  set("output destination", "silent");

  // Assembly of a sparse matrix
  AssembleSparseMatrices();  

  // Assembly of a Poisson problem (this should really be in FEM)
  AssemblePoissonMatrix();  

  // Sparse matrix - vector multiplication
  MatrixVectorMultiply();  

  return 0;
}
