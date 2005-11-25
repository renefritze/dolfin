// Copyright (C) 2005 Garth N. Wells.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-10-23
// Last changed: 2005

#ifndef __NONLINEAR_SOLVER_H
#define __NONLINEAR_SOLVER_H

#include <petscsnes.h>

#include <dolfin/constants.h>
#include <dolfin/NonlinearFunction.h>
#include <dolfin/Vector.h>
#include <dolfin/Matrix.h>

namespace dolfin
{

  /// This class defines the interface of nonlinear solvers for
  /// equations of the form F(u) = 0.
  
  class NonlinearSolver
  {
  public:

    /// Initialise nonlinear solver for a given nonlinear function
    NonlinearSolver();

    /// Destructor
    ~NonlinearSolver();
  
    /// Solve nonlinear problem F(u) = 0
    uint solve(NonlinearFunction& nonlinear_function, Vector& x);

    /// Set maximum number of Netwon iterations
    void setMaxiter(int maxiter);

    /// Set relative convergence tolerance: du_i / du_0 < rtol
    void setRtol(real rtol);

    /// Set successive convergence tolerance:  du_i+1 / du_i < stol
    void setStol(real stol);

    /// Set absolute convergence tolerance:  du_i < atol
    void setAtol(real atol);

  private:

    /// Function passed to PETSc to form RHS vector F(u)
    static int formRHS(SNES snes, Vec x, Vec f, void* nlfunc);

    /// Function passed to PETSc to form Jacobian (stiffness matrix) F'(u) = dF(u)/du
    static int formJacobian(SNES snes, Vec x, Mat* AA, Mat* BB, MatStructure *flag, void* nlfunc);

    /// Function passed to PETSc to form RHS vector and Jacobian 
    static int formSystem(SNES snes, Vec x, Vec f, void* nlfunc);

    /// Dummy function passed to PETSc for computing Jacobian 
    static int formDummy(SNES snes, Vec x, Mat* AA, Mat* BB, MatStructure *flag, void* nlfunc);

    /// Monitor function for nonlinear solver 
    static int monitor(SNES snes, int iter, real fnorm, void* dummy);

    // PETSc nonlinear solver pointer
    SNES snes;

  };

}

#endif
