// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __TIME_STEPPER_H
#define __TIME_STEPPER_H

#include <dolfin/constants.h>

namespace dolfin {

  class ODE;

  /// Used by the ODE solver to integrate an ODE over an interval of
  /// given length.
  ///
  /// This is where the real work takes place (well, actually most of
  /// it takes place in the time slab or even in the local elements),
  /// whereas the responsibility of the ODE solver is also to solve
  /// the dual problem (using this class), compute stability factors
  /// and compute error estimates.

  class TimeStepper {
  public:

    static void solve(ODE& ode, real t0, real t1);

  private:

  };

}

#endif
