// Copyright (C) 2006 Garth N. Wells.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg 2006.
//
// First added:  2006-06-23
// Last changed: 2006-07-04

#ifndef __UBLAS_ILU_PRECONDITIONER_H
#define __UBLAS_ILU_PRECONDITIONER_H

#include <dolfin/ublas.h>
#include <dolfin/Array.h>
#include <dolfin/uBlasPreconditioner.h>

namespace dolfin
{
  
  template<class Mat> class uBlasMatrix;

  /// This class implements an incomplete LU factorization (ILU)
  /// preconditioner for the uBlas Krylov solver.

  class DenseVector;

  class uBlasILUPreconditioner : public uBlasPreconditioner
  {
  public:

    /// Constructor
    uBlasILUPreconditioner(const uBlasMatrix<ublas_sparse_matrix>& A);

    /// Destructor
    ~uBlasILUPreconditioner();

    /// Solve linear system Ax = b approximately
    void solve(DenseVector& x, const DenseVector& b) const;

  private:

    // Initialize preconditioner
    void init(const uBlasMatrix<ublas_sparse_matrix>& A);

    // Preconditioner matrix
    uBlasMatrix<ublas_sparse_matrix> M;

    // Diagonal
    Array<uint> diagonal;

  };

}

#endif
