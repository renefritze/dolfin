// Copyright (C) 2003-2006 Johan Jansson and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003
// Last changed: 2006-05-29

#include <dolfin/dolfin_log.h>
#include <dolfin/ParameterSystem.h>
#include <dolfin/ODE.h>
#include <dolfin/TimeStepper.h>
#include <dolfin/ODESolver.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void ODESolver::solve(ODE& ode)
{
  dolfin_begin("Solving ODE on time interval (0, %g).", ode.endtime());

  // Temporary fix until we fix the dual problem again
  TimeStepper::solve(ode);

  cout << "Not computing an error estimate. " 
       << "The solution may be inaccurate." << endl;

  dolfin_end();
}
//-----------------------------------------------------------------------------

// FIXME: BROKEN

/*

//-----------------------------------------------------------------------------
void ODESolver::solve(ODE& ode)
{
  Function u, phi;
  solve(ode, u, phi);
}
//-----------------------------------------------------------------------------
void ODESolver::solve(ODE& ode, Function& u)
{
  Function phi;
  solve(ode, u, phi);
}
//-----------------------------------------------------------------------------
void ODESolver::solve(ODE& ode)
{
  // Check if we should solve the dual problem
  bool solve_dual = get("ODE solve dual problem");

  dolfin_begin("Solving ODE");  

  // Solve primal problem
  solvePrimal(ode, u);

  // Solve dual problem
  if ( solve_dual )
    solveDual(ode, u, phi);
  else
    cout << "Not solving the dual problem as requested." << endl;

  cout << "Not computing an error estimate. " 
       << "The solution may be inaccurate." << endl;

  dolfin_end();
}
//-----------------------------------------------------------------------------
void ODESolver::solvePrimal(ODE& ode)
{
  dolfin_begin("Solving primal problem");
  
  // Initialize primal solution
  //u.init(ode.size());
  //u.rename("u", "primal");
  
  // Solve primal problem
  TimeStepper::solve(ode);

  dolfin_end();
}
//-----------------------------------------------------------------------------
void ODESolver::solveDual(ODE& ode, Function& u, Function& phi)
{ 
  dolfin_begin("Solving dual problem");
  
  // FIXME: BROKEN

  // Create dual problem
  Dual dual(ode, u);
  
  // Initialize dual solution phi
  phi.init(ode.size());
  phi.rename("phi", "dual");
  
  // Solve dual problem
  if ( get("ODE use new ode solver") )
    TimeStepper::solve(ode);
  else
    TimeStepper::solve(dual, phi);

  dolfin_end();
}
//-----------------------------------------------------------------------------

*/
