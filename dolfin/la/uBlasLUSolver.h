// Copyright (C) 2006 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg 2006.
// 
// First added:  2006-05-31
// Last changed: 2006-08-22

#ifndef __UBLAS_LU_SOLVER_H
#define __UBLAS_LU_SOLVER_H

#include "ublas.h"
#include <dolfin/parameter/Parametrized.h>

namespace dolfin
{

  /// Forward declarations
  class uBlasVector;
  class uBlasKrylovMatrix;
  template<class Mat> class uBlasMatrix;

  /// This class implements the direct solution (LU factorization) of
  /// linear systems of the form Ax = b using uBlas data types. Dense
  /// matrices are solved using uBlas LU factorisation, and sparse matrices
  /// are solved using UMFPACK (http://www.cise.ufl.edu/research/sparse/umfpack/)
  /// is installed. Matrices can also be inverted.
    
  class uBlasLUSolver : public Parametrized
  {

  public:
    
    /// Constructor
    uBlasLUSolver();

    /// Destructor
    ~uBlasLUSolver();

    /// Solve linear system Ax = b for a dense matrix
    virtual uint solve(const uBlasMatrix<ublas_dense_matrix>& A, uBlasVector& x, const uBlasVector& b);

    /// Solve linear system Ax = b for a sparse matrix using UMFPACK if installed
    virtual uint solve(const uBlasMatrix<ublas_sparse_matrix>& A, uBlasVector& x, const uBlasVector& b);

    /// LU-factor sparse matrix A if UMFPACK is installed
    virtual uint factorize(const uBlasMatrix<ublas_sparse_matrix>& A);

    /// Solve factorized system (UMFPACK).
    virtual uint factorized_solve(uBlasVector& x, const uBlasVector& b);

    /// Solve linear system Ax = b for a Krylov matrix
    void solve(const uBlasKrylovMatrix& A, uBlasVector& x, const uBlasVector& b);

    /// Solve linear system Ax = b in place (A is dense)
    uint solveInPlaceUBlas(uBlasMatrix<ublas_dense_matrix>& A, uBlasVector& x, const uBlasVector& b) const;

    /// Compute the inverse of A (A is dense or sparse)
    template<class Mat>
    void invert(Mat& A) const;

  private:
    
    /// Check status flag returned by an UMFPACK function
    void check_status(long int status, std::string function) const;

    /// General uBlas LU solver which accepts both vector and matrix right-hand sides
    template<class Mat, class B>
    uint solveInPlace(Mat& A, B& X) const;

    // Temporary data for LU factorization of sparse ublas matrix (umfpack only)
#ifdef HAS_UMFPACK
    bool has_factorized_matrix;
    uint mat_dim;
    void* Numeric;
    long int* Rp;
    long int* Ri;
    double* Rx;   
#endif

    // Temporary data for LU factorization of a uBlasKrylovMatrix
    uBlasMatrix<ublas_dense_matrix>* AA;
    uBlasVector* ej;
    uBlasVector* Aj;
    
  };
  //---------------------------------------------------------------------------
  // Implementation of template functions
  //---------------------------------------------------------------------------
  template<class Mat>
  void uBlasLUSolver::invert(Mat& A) const
  {
    const uint M = A.size1();
    dolfin_assert(M == A.size2());
  
    // Create indentity matrix
    Mat X(M, M);
    X.assign(ublas::identity_matrix<real>(M));

    // Solve
    solveInPlace(A, X);

    A.assign_temporary(X);
  }
  //-----------------------------------------------------------------------------
  template<class Mat, class B>
  dolfin::uint uBlasLUSolver::solveInPlace(Mat& A, B& X) const
  {
    const uint M = A.size1();
    dolfin_assert( M == A.size2() );
  
    // Create permutation matrix
    ublas::permutation_matrix<std::size_t> pmatrix(M);

    // Factorise (with pivoting)
    uint singular = ublas::lu_factorize(A, pmatrix);
    if( singular > 0)
      error("Singularity detected in uBlas matrix factorization on line %u.", singular-1); 

    // Back substitute 
    ublas::lu_substitute(A, pmatrix, X);

    return 1;
  }
  //-----------------------------------------------------------------------------

}

#endif
