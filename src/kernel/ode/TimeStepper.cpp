// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Johan Jansson 2003, 2004.

#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_settings.h>
#include <dolfin/timeinfo.h>
#include <dolfin/ODE.h>
#include <dolfin/Sample.h>
#include <dolfin/TimeSlab.h>
#include <dolfin/SimpleTimeSlab.h>
#include <dolfin/RecursiveTimeSlab.h>
#include <dolfin/TimeStepper.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
TimeStepper::TimeStepper(ODE& ode, Function& function) :
  no_samples(dolfin_get("number of samples")), N(ode.size()), t(0),
  T(ode.endtime()), partition(N), adaptivity(ode), u(ode, function),
  f(ode, u), fixpoint(u, f), file(u.label() + ".m"), p("Time-stepping"),
  _finished(false)
{
  dolfin_warning("ODE solver is EXPERIMENTAL..");

  // Start timing
  tic();
}
//-----------------------------------------------------------------------------
TimeStepper::~TimeStepper()
{
  // Display status report
  cout << "Solution computed in " << toc() << " seconds." << endl;
  fixpoint.report();
}
//-----------------------------------------------------------------------------
void TimeStepper::solve(ODE& ode, Function& function)
{
  // Create a TimeStepper object
  TimeStepper timeStepper(ode, function);

  // Do time stepping
  while ( !timeStepper.finished() )
    timeStepper.step();
}
//-----------------------------------------------------------------------------
real TimeStepper::step()
{
  cout << "Creating new time slab" << endl;
  
  TimeSlab* timeslab = 0;

  // Repeat until the time slab has converged
  while (true)
  {
    // Create a new time slab
    if ( t == 0.0 )
      timeslab = new SimpleTimeSlab(t, T, u, adaptivity);
    else
      timeslab = new RecursiveTimeSlab(t, T, u, f, adaptivity, fixpoint, partition, 0);
    
    timeslab->show();
  
    // Solve system using damped fixed point iteration
    if ( fixpoint.iterate(*timeslab) )
      break;

    // Time slab did not converge, so we throw away the time slab and try again
    decreaseTimeStep();
    delete timeslab;
  }
  
  // Update time
  t = timeslab->endtime();
  
  // Save solution
  save(*timeslab);
  
  // Prepare for next time slab
  shift();
  
  // Update progress
  p = t / T;

  // Check if we are done
  if ( timeslab->finished() )
  {
    _finished = true;
    p = 1.0;
  }
  
  // Delete time slab
  delete timeslab;
  
  // Return end time
  return t;
}
//-----------------------------------------------------------------------------
bool TimeStepper::finished() const
{
  return _finished;
}
//-----------------------------------------------------------------------------
void TimeStepper::shift()
{
  real TOL = adaptivity.tolerance();
  real kmax = adaptivity.maxstep();
  real kfixed = adaptivity.fixed();

  // Update residuals and time steps
  for (unsigned int i = 0; i < u.size(); i++)
  {
    // Get last element
    Element* element = u.last(i);
    dolfin_assert(element);

    // Compute residual
    real r = element->computeResidual(f);

    // Compute new time step
    real k = element->computeTimeStep(TOL, r, kmax);

    // Update regulator
    adaptivity.regulator(i).update(k, kmax, kfixed);
  }
  
  // Shift solution
  u.shift(t);
  
  // Shift adaptivity
  adaptivity.shift();
}
//-----------------------------------------------------------------------------
void TimeStepper::save(TimeSlab& timeslab)
{
  // Compute time of first sample within time slab
  real K = T / static_cast<real>(no_samples);
  real t = ceil(timeslab.starttime()/K) * K;

  // Save samples
  while ( t < timeslab.endtime() )
  {
    Sample sample(u, f, t);
    file << sample;
    t += K;
  }
  
  // Save end time value
  if ( timeslab.finished() ) {
    Sample sample(u, f, timeslab.endtime());
    file << sample;
  }
}
//-----------------------------------------------------------------------------
void TimeStepper::decreaseTimeStep()
{
  // FIXME: Maybe this should be a parameter
  adaptivity.decreaseTimeStep(0.5);

  // Throw away solution values for current time slab
  u.reset();
}
//-----------------------------------------------------------------------------
