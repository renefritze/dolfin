// Copyright (C) 2008 Kent-Andre Mardal.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2008.
//
// Last changed: 2008-08-25

#ifdef HAS_TRILINOS

#ifndef __EPETRA_KRYLOV_SOLVER_H
#define __EPETRA_KRYLOV_SOLVER_H

#include <dolfin/common/types.h>
#include "GenericLinearSolver.h"
#include "enums_la.h"
#include "EpetraPreconditioner.h"

namespace dolfin 
{

  /// Forward declarations
  class GenericMatrix;
  class GenericVector;
  class EpetraMatrix;
  class EpetraVector;
  class EpetraKrylovMatrix;

  /// This class implements Krylov methods for linear systems
  /// of the form Ax = b. It is a wrapper for the Krylov solvers
  /// of Epetra.
  
  class EpetraKrylovSolver : public GenericLinearSolver
  {
  public:

    /// Create Krylov solver for a particular method and preconditioner
    EpetraKrylovSolver(dolfin::SolverType method=default_solver,
                       dolfin::PreconditionerType pc=default_pc);

    /// Create Krylov solver for a particular method and EpetraPreconditioner
    EpetraKrylovSolver(dolfin::SolverType method, EpetraPreconditioner& prec);

    /// Destructor
    ~EpetraKrylovSolver();

    /// Solve linear system Ax = b and return number of iterations
    uint solve(const GenericMatrix& A, GenericVector& x, const GenericVector& b);

    /// Solve linear system Ax = b and return number of iterations
    uint solve(const EpetraMatrix& A, EpetraVector& x, const EpetraVector& b);
    
    /// Display solver data
    void disp() const;

  private:

    SolverType         method; 
    PreconditionerType pc_type; 
    EpetraPreconditioner* prec; 

  };

}

#endif 

#endif 
