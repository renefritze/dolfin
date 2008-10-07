// Copyright (C) 2005-2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2005-01-28
// Last changed: 2006-08-08

#ifndef __MONO_ADAPTIVE_NEWTON_SOLVER_H
#define __MONO_ADAPTIVE_NEWTON_SOLVER_H

#include <dolfin/common/types.h>
#include <dolfin/la/uBLASVector.h>
#include "MonoAdaptiveJacobian.h"
#include "TimeSlabSolver.h"

namespace dolfin
{
  
  class uBLASKrylovSolver;
  class UmfpackLUSolver;
  class KrylovSolver;
  class LUSolver;
  class ODE;
  class MonoAdaptiveTimeSlab;
  class NewMethod;

  /// This class implements Newton's method on mono-adaptive time
  /// slabs. In each iteration, the system F(x) is evaluated at the
  /// current solution and then the linear system A dx = b is solved
  /// for the increment dx with A = F' the Jacobian of F and b = -F(x)

  class MonoAdaptiveNewtonSolver : public TimeSlabSolver
  {
  public:
    
    /// Constructor
    MonoAdaptiveNewtonSolver(MonoAdaptiveTimeSlab& timeslab, bool implicit = false);

    /// Destructor
    ~MonoAdaptiveNewtonSolver();

  protected:

    /// Start iterations (optional)
    void start();
    
    // Make an iteration
    double iteration(double tol, uint iter, double d0, double d1);

    /// Size of system
    uint size() const;

  private:

    // Evaluate -F(x) at current x
    void Feval(double*  F);

    // Evaluate -F(x) for explicit system: u' = f
    void FevalExplicit(double* F);

    // Evaluate -F(x) for implicit system: Mu' = f
    void FevalImplicit(double* F);
	
    // Choose  linear solver
    void chooseLinearSolver();

    // Numerical evaluation of the Jacobian used for testing
    void debug();
    
    bool implicit;  // True if ODE is implicit
    bool piecewise; // True if M is piecewise constant

    MonoAdaptiveTimeSlab& ts;    // The time slab;
    MonoAdaptiveJacobian A;      // Jacobian of time slab system
    uBLASVector dx;              // Increment for Newton's method
    uBLASVector b;               // Right-hand side b = -F(x)
    double* btmp;                // Copy of right-hand side b = -F(x)
    double* Mu0;                 // Precomputed product M*u0 for implicit system
    uBLASKrylovSolver* krylov;   // Iterative linear solver
    UmfpackLUSolver* lu;         // Direct linear solver
    KrylovSolver* krylov_g;      // Iterative linear solver (general)
    LUSolver* lu_g;              // Direct linear solver (general)

  };

}

#endif
