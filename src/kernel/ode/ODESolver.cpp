// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/constants.h>
#include <dolfin/dolfin_log.h>
#include <dolfin/ODE.h>
#include <dolfin/TimeStepper.h>
#include <dolfin/ODESolver.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void ODESolver::solve(ODE& ode)
{
  // Eventually, this is where we will put the adaptive algorithm,
  // including repeated solution of the primal and dual problems,
  // computation stability factors and error estimates.

  dolfin_start("Solving ODE");

  TimeStepper::solve(ode, 0.0, ode.endtime());

  dolfin_end();
}
//-----------------------------------------------------------------------------
