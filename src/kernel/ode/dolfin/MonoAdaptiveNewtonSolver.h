// Copyright (C) 2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __MONO_ADAPTIVE_NEWTON_SOLVER_H
#define __MONO_ADAPTIVE_NEWTON_SOLVER_H

#include <dolfin/constants.h>
#include <dolfin/NewGMRES.h>
#include <dolfin/NewVector.h>
#include <dolfin/MonoAdaptiveJacobian.h>
#include <dolfin/TimeSlabSolver.h>

namespace dolfin
{

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

    /// Solve system
    void solve();

  protected:

    /// Start iterations (optional)
    void start();
    
    // Make an iteration
    real iteration();

  private:

    // Evaluate b = -F(x) at current x
    void beval();

    // Numerical evaluation of the Jacobian used for testing
    void debug();

    MonoAdaptiveTimeSlab& ts; // The time slab;
    MonoAdaptiveJacobian A;   // Jacobian of time slab system
    NewVector dx;             // Increment for Newton's method
    NewVector b;              // Right-hand side -F(x)
    NewGMRES solver;          // GMRES solver
    bool implicit;            // True if ODE is implicit
    
  };

}

#endif
