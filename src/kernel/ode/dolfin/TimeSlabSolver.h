// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-01-05
// Last changed: 2005-10-24

#ifndef __TIME_SLAB_SOLVER_H
#define __TIME_SLAB_SOLVER_H

namespace dolfin
{
  class ODE;
  class Method;
  class TimeSlab;
  
  /// This is the base class for solvers of the system of equations
  /// defined on time slabs.

  class TimeSlabSolver
  {
  public:
    
    /// Constructor
    TimeSlabSolver(TimeSlab& timeslab);

    /// Destructor
    virtual ~TimeSlabSolver();

    /// Solve system
    bool solve();

  protected:

    /// Start iterations (optional)
    virtual void start();

    /// End iterations (optional)
    virtual void end();

    /// Make an iteration
    virtual real iteration(uint iter, real tol) = 0;

    /// Size of system
    virtual uint size() const = 0;

    // The ODE
    ODE& ode;

    // The method
    const Method& method;
    
    // Tolerance for iterations (max-norm)
    real tol;

    // Maximum number of iterations
    uint maxiter;

    // True if we should monitor the convergence
    bool monitor;

    // Number of time slabs systems solved
    uint num_timeslabs;

    // Number of iterations made
    uint num_iterations;

  };

}

#endif
