// Copyright (C) 2003-2005 Johan Jansson and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003
// Last changed: 2005-10-29

#include <cmath>
#include <string>
#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_settings.h>
#include <dolfin/timeinfo.h>
#include <dolfin/ODE.h>
#include <dolfin/ReducedModel.h>
#include <dolfin/Sample.h>
#include <dolfin/MonoAdaptiveTimeSlab.h>
#include <dolfin/MultiAdaptiveTimeSlab.h>
#include <dolfin/TimeStepper.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
TimeStepper::TimeStepper(ODE& ode) :
  N(ode.size()), t(0), T(ode.endtime()),
  ode(ode), timeslab(0), file(dolfin_get("file name")),
  p("Time-stepping"), stopped(false), _finished(false),
  save_solution(dolfin_get("save solution")),
  solve_dual(dolfin_get("solve dual problem")),
  adaptive_samples(dolfin_get("adaptive samples")),
  no_samples(dolfin_get("number of samples")),
  sample_density(dolfin_get("sample density"))
{
  dolfin_warning("ODE solver is EXPERIMENTAL.");

  // Create time slab
  std::string method = dolfin_get("method");
  if ( method == "mcg" || method == "mdg" )
  {
    timeslab = new MultiAdaptiveTimeSlab(ode);
  }
  else    
  {
    timeslab = new MonoAdaptiveTimeSlab(ode);
  }
}
//-----------------------------------------------------------------------------
TimeStepper::~TimeStepper()
{
  if ( timeslab ) delete timeslab;
}
//-----------------------------------------------------------------------------
void TimeStepper::solve(ODE& ode)
{
  // Start timing
  tic();  
  
  // Check if we should create a reduced model (automatic modeling)
  if ( dolfin_get("automatic modeling") )
  {
    dolfin_info("Creating reduced model (automatic modeling).");
    dolfin_error("Automatic modeling temporarily broken.");

    // Create the reduced model
    //ReducedModel reducedModel(ode);

    // Create a time stepper object
    //TimeStepper timeStepper(reducedModel, u);
    
    // Do time stepping
    //while ( !timeStepper.finished() )
    //  timeStepper.step();
  }
  else
  {
    // Create a time stepper object
    TimeStepper timeStepper(ode);
    
    // Do time stepping

    while ( !timeStepper.finished() && !timeStepper.stopped )
      timeStepper.step();
  }

  // Report elapsed time
  dolfin_info("Solution computed in %.3f seconds.", toc());
}
//-----------------------------------------------------------------------------
real TimeStepper::step()
{
  // FIXME: Change type of time slab if solution does not converge

  // Check if this is the first time step
  const bool first = t < DOLFIN_EPS;

  // Iterate until solution is accepted
  const real a = t;
  while ( true )
  {
    // Build time slab
    t = timeslab->build(a, T);
    //timeslab->disp();
    
    // Solve time slab system
    if ( !timeslab->solve() )
    {
      stopped = true;
      break;
    }
    
    // Check if solution can be accepted
    if ( timeslab->check(first) )
      break;
    
    dolfin_info("Rejecting time slab K = %.3e, trying again.", timeslab->length());
  }

  // Save solution
  save();

  // Check if solution was stopped
  if ( stopped )
    dolfin_warning1("Solution stopped at t = %.3e.", t);

  // Update for next time slab
  if ( !timeslab->shift() )
  {
    dolfin_info("ODE solver stopped on user's request.");
    stopped = true;
  }

  // Update progress
  if ( !stopped )
    p = t / T;
  else
    p.stop();

  return t;
}
//-----------------------------------------------------------------------------
bool TimeStepper::finished() const
{
  return t >= T;
}
//-----------------------------------------------------------------------------
void TimeStepper::save()
{
  // Check if we should save the solution
  if ( !save_solution )
    return;

  // Choose method for saving the solution
  if ( adaptive_samples )
    saveAdaptiveSamples();
  else
    saveFixedSamples();
}
//-----------------------------------------------------------------------------
void TimeStepper::saveFixedSamples()
{
  // Get start time and end time of time slab
  real t0 = timeslab->starttime();
  real t1 = timeslab->endtime();

  // Save initial value
  if ( t0 == 0.0 )
  {
    //Sample sample(*timeslab, 0.0, u.name(), u.label());
    Sample sample(*timeslab, 0.0, "u", "unknown");
    file << sample;
    ode.save(sample);
  }

  // Compute distance between samples
  real K = T / static_cast<real>(no_samples);
  real t = floor(t0/K - 0.5) * K;

  // Save samples
  while ( true )
  {
    t += K;

    if ( (t - DOLFIN_EPS) < t0 )
      continue;

    if ( (t - DOLFIN_EPS) > t1 )
      break;

    if ( fabs(t - t1) < DOLFIN_EPS )
      t = t1;

    //Sample sample(*timeslab, t, u.name(), u.label());
    Sample sample(*timeslab, t, "u", "unknown");
    file << sample;
    ode.save(sample);
  }
}
//-----------------------------------------------------------------------------
void TimeStepper::saveAdaptiveSamples()
{
  // Get start time and end time of time slab
  real t0 = timeslab->starttime();
  real t1 = timeslab->endtime();

  // Save initial value
  if ( t0 == 0.0 )
  {
    //Sample sample(*timeslab, 0.0, u.name(), u.label());
    Sample sample(*timeslab, 0.0, "u", "unknown");
    file << sample;
    ode.save(sample);
  }

  // Compute distance between samples
  dolfin_assert(sample_density >= 1);
  real k = (t1 - t0) / static_cast<real>(sample_density);
  
  // Save samples
  for (unsigned int n = 0; n < sample_density; ++n)
  {
    // Compute time of sample, and make sure we get the end time right
    real t = t0 + static_cast<real>(n + 1)*k;
    if ( n == (sample_density - 1) )
      t = t1;
    
    // Create and save the sample
    //Sample sample(*timeslab, t, u.name(), u.label());
    Sample sample(*timeslab, t, "u", "unknown");
    file << sample;
    ode.save(sample);
  }
}
//-----------------------------------------------------------------------------
