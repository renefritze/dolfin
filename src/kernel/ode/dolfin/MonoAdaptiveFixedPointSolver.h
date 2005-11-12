// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-01-28
// Last changed: 2005-11-11

#ifndef __MONO_ADAPTIVE_FIXED_POINT_SOLVER_H
#define __MONO_ADAPTIVE_FIXED_POINT_SOLVER_H

#include <dolfin/constants.h>
#include <dolfin/TimeSlabSolver.h>

namespace dolfin
{
  class MonoAdaptiveTimeSlab;
  
  /// This class implements fixed-point iteration on mono-adaptive
  /// time slabs. In each iteration, the solution is updated according
  /// to the fixed-point iteration x = g(x).

  class MonoAdaptiveFixedPointSolver : public TimeSlabSolver
  {
  public:

    /// Constructor
    MonoAdaptiveFixedPointSolver(MonoAdaptiveTimeSlab& timeslab);

    /// Destructor
    ~MonoAdaptiveFixedPointSolver();

    /// Solve system
//     bool solve();

  protected:

    // Make an iteration
    real iteration(uint iter, real tol);

    /// Size of system
    uint size() const;

  private:

    // The time slab
    MonoAdaptiveTimeSlab& ts;

    // Old values at right end-point used to compute the increment
    real* xold;

  };

}

#endif
