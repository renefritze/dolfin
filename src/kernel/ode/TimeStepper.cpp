// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// - Updates by Johan Jansson (2003)

#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>

#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_settings.h>
#include <dolfin/File.h>
#include <dolfin/ODE.h>
#include <dolfin/RHS.h>
#include <dolfin/Partition.h>
#include <dolfin/TimeSlabData.h>
#include <dolfin/SimpleTimeSlab.h>
#include <dolfin/RecursiveTimeSlab.h>
#include <dolfin/TimeSlab.h>
#include <dolfin/TimeSlabSample.h>
#include <dolfin/TimeStepper.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void TimeStepper::solve(ODE& ode, real t0, real t1)
{
  // Get parameters
  int no_samples = dolfin_get("number of samples");

  // Create data for time-stepping
  TimeSlabData data(ode);
  Partition partition(ode.size());
  RHS f(ode, data);
  TimeSlab* timeslab = 0;
  real t = t0;

  // Create file for storing the solution
  File file("solution.m");

  // The time-stepping loop
  Progress p("Time-stepping");
  while ( true ) {
    
    // Create a new time slab
    if ( t == t0 )
      timeslab = new SimpleTimeSlab(t, t1, f, data);
    else
      timeslab = new RecursiveTimeSlab(t, t1, f, data, partition, 0);
    
    cout << "Created a time slab: " << *timeslab << endl;
    
    // Iterate a couple of times on the time slab
    for (int i = 0; i < 2; i++)
      timeslab->update(f, data);

    // Save solution
    save(*timeslab, data, f, file, t0, t1, no_samples);
    
    // Check if we are done
    if ( timeslab->finished() )
      break;

    // Update time
    t = timeslab->endtime();

    // Update progress
    p = (t - t0) / (t1 - t0);

    // Prepare for next time slab
    data.shift(*timeslab, f);

    // Delete time slab
    delete timeslab;
  }

}
//-----------------------------------------------------------------------------
void TimeStepper::save(TimeSlab& timeslab, TimeSlabData& data, RHS& f,
		       File& file, real t0, real t1, int no_samples)
{
  // Compute time of first sample within time slab
  real K = (t1 - t0) / static_cast<real>(no_samples);
  real t =  t0 + ceil((timeslab.starttime()-t0)/K) * K;

  // Save samples
  while ( t < timeslab.endtime() )
  {
    // Create a sample of the solution
    TimeSlabSample sample(timeslab, data, f, t);
    
    // Save solution to file
    file << sample;

    // Step to next sample
    t += K;
  }
}
//-----------------------------------------------------------------------------
