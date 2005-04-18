// Copyright (C) 2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_settings.h>
#include <dolfin/NewTimeSlab.h>
#include <dolfin/TimeSlabSolver.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
TimeSlabSolver::TimeSlabSolver(NewTimeSlab& timeslab)
  : ode(timeslab.ode), method(*timeslab.method), tol(0.0), maxiter(0),
    monitor(dolfin_get("monitor convergence"))
  
{
  // Get tolerance
  const real TOL = dolfin_get("tolerance");
  if ( dolfin_parameter_changed("discrete tolerance") )
  {
    tol = dolfin_get("discrete tolerance");
  }
  else
  {
    const real alpha = dolfin_get("discrete tolerance factor");
    tol = alpha * TOL;
  }

  cout << "Using discrete tolerance tol = " << tol << "." << endl;

  // Get maximum number of iterations
  maxiter = dolfin_get("maximum iterations");
}
//-----------------------------------------------------------------------------
TimeSlabSolver::~TimeSlabSolver()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
bool TimeSlabSolver::solve()
{
  start();

  for (uint iter = 0; iter < maxiter; iter++)
  {
    // Do one iteration
    real increment = iteration();
    if ( monitor )
      dolfin_info("--- iter = %d: increment = %.3e", iter, increment);
    
    // Check convergenge
    if ( increment < tol )
    {
      end();
      if ( monitor )
	dolfin_info("Time slab system converged in %d iterations.", iter + 1);
      return true;
    }
  }

  end();
  dolfin_warning("Time slab system did not converge, solution stopped.");
  return false;
}
//-----------------------------------------------------------------------------
void TimeSlabSolver::start()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void TimeSlabSolver::end()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
