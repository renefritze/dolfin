// Copyright (C) 2005 Johan Jansson.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg, 2005.
// Modified by Johan Hoffman, 2005.

#ifndef __NEW_GMRES_H
#define __NEW_GMRES_H

#include <petscksp.h>
#include <dolfin/constants.h>
#include <dolfin/NewPreconditioner.h>
#include <dolfin/NewLinearSolver.h>

namespace dolfin
{

  /// This class implements the GMRES method for linear systems
  /// of the form Ax = b. It is a wrapper for the GMRES solver
  /// of PETSc.
  
  class NewGMRES : public NewLinearSolver
  {
  public:

    /// Create GMRES solver
    NewGMRES();

    /// Destructor
    ~NewGMRES();

    /// Solve linear system Ax = b
    void solve(const NewMatrix& A, NewVector& x, const NewVector& b);

    /// Solve linear system Ax = b (matrix-free version)
    void solve(const VirtualMatrix& A, NewVector& x, const NewVector& b);

    /// FIXME: Options below should be moved to some parameter system,
    /// FIXME: not very nice to have a long list of setFoo() functions.

    /// Change whether solver should report the number iterations
    void setReport(bool report);

    /// Change rtol
    void setRtol(real rtol);
      
    /// Change abstol
    void setAtol(real atol);
      
    /// Change dtol
    void setDtol(real dtol);
      
    /// Change maxiter
    void setMaxiter(int maxiter);

    /// Set preconditioner
    void setPreconditioner(NewPreconditioner &pc);

    /// Return PETSc solver pointer
    KSP solver();

    /// Display GMRES solver data
    void disp() const;
     
  private:

    // Create preconditioner matrix for virtual matrix
    void createVirtualPreconditioner(const VirtualMatrix& A);

    // True if we should report the number of iterations
    bool report;

    // PETSc solver pointer
    KSP ksp;

    // Diagonal matrix used for preconditioning with virtual matrix
    Mat B;

  };

}

#endif
