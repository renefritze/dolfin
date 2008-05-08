// Copyright (C) 2007 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Ola Skavhaug 2008.
//
// First added:  2007-07-03
// Last changed: 2008-04-11

#ifndef __KRYLOV_SOLVER_H
#define __KRYLOV_SOLVER_H

#include <dolfin/parameter/Parametrized.h>
#include "LinearSolver.h"
#include "uBlasSparseMatrix.h"
#include "uBlasDenseMatrix.h"
#include "PETScMatrix.h"

#include "KrylovMethod.h"
#include "Preconditioner.h"

namespace dolfin
{

  /// This class defines an interface for a Krylov solver. The underlying 
  /// Krylov solver type is defined in default_type.h.
  
  class KrylovSolver : public LinearSolver, public Parametrized
  {
  public:
    
    /// Create Krylov solver
    KrylovSolver(KrylovMethod method=default_method, Preconditioner pc=default_pc)
      : method(method), pc(pc), ublassolver(0), petscsolver(0) {}
    
    /// Destructor
    ~KrylovSolver()
    {
      delete ublassolver; 
      delete petscsolver; 
    }
    
    /// Solve linear system Ax = b
    uint solve(const GenericMatrix& A, GenericVector& x, const GenericVector& b)
    { 
      if (A.has_type<uBlasSparseMatrix>())
      {
        if (!ublassolver)
          ublassolver = new uBlasKrylovSolver(method, pc);
        return ublassolver->solve(A.down_cast<uBlasSparseMatrix>(), x.down_cast<uBlasVector>(), b.down_cast<uBlasVector>());
      }

      if (A.has_type<uBlasDenseMatrix>())
      {
        if (!ublassolver)
          ublassolver = new uBlasKrylovSolver(method, pc);
        return ublassolver->solve(A.down_cast<uBlasDenseMatrix>(), x.down_cast<uBlasVector>(), b.down_cast<uBlasVector>());
      }

#ifdef HAS_PETSC
      if (A.has_type<PETScMatrix>())
      {
        if (!petscsolver)
          petscsolver = new PETScKrylovSolver(method, pc);
        return petscsolver->solve(A.down_cast<PETScMatrix >(), x.down_cast<PETScVector>(), b.down_cast<PETScVector>());
      }
#endif

      error("No default LU solver for given backend");
      return 0;
    }
    
  private:
    
    // Krylov method
      KrylovMethod method;

    // Preconditioner type
    Preconditioner pc;

    // uBLAS solver
    uBlasKrylovSolver* ublassolver;

    // PETSc solver
#ifdef HAS_PETSC
    PETScKrylovSolver* petscsolver;
#else
    int* petscsolver;
#endif
    
  };

}

#endif
