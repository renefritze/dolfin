// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __ODE_SOLVER_H
#define __ODE_SOLVER_H

#include <dolfin/constants.h>

namespace dolfin {

  class ODE;

  /// Solves a given ODE of the form
  ///
  ///     u'(t) = f(u(t),t) on (0,T],
  ///         
  ///     u(0)  = u0,
  ///
  /// where u(t) is a vector of length N.
  
  class ODESolver {
  public:

    static void solve(ODE& ode);
    //static void solve(ODE& ode, Function& u);
    //static void solve(ODE& ode, Function& u, Function& phi);

  private:

    //static void solvePrimal(ODE& ode, Function& u);
    //static void solveDual(ODE& ode, Function& u, Function& phi);

  };

}

#endif
