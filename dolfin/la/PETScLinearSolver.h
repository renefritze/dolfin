// Copyright (C) 2004-2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2006.
//
// First added:  2004-06-19
// Last changed: 2007-05-15

#ifndef __PETSC_LINEAR_SOLVER_H
#define __PETSC_LINEAR_SOLVER_H

#ifdef HAS_PETSC

#include "PETScObject.h"

namespace dolfin
{

  /// Forward declarations
  class PETScMatrix;
  class PETScVector;
  class GenericMatrix;
  class GenericVector;

  /// This class defines the interfaces for PETSc linear solvers for
  /// systems of the form Ax = b.

  class PETScLinearSolver : public PETScObject
  {
  public:

    /// Constructor
    PETScLinearSolver(){}

    /// Destructor
    virtual ~PETScLinearSolver(){}

    /// Solve linear system Ax = b
    virtual unsigned int solve(const GenericMatrix& A, GenericVector& x, const GenericVector& b);

    /// Solve linear system Ax = b
    virtual unsigned int solve(const PETScMatrix& A, PETScVector& x, const PETScVector& b) = 0;

  };

}

#endif

#endif
