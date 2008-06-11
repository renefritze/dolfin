// Copyright (C) 2003-2008 Johan Jansson and Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003
// Last changed: 2008-01-16

#ifndef __TIME_STEPPER_H
#define __TIME_STEPPER_H

#include <dolfin/common/types.h>
#include <dolfin/io/File.h>
#include "ODESolution.h"

namespace dolfin
{

  class ODE;
  class TimeSlab;

  /// TimeStepper computes the solution of a given ODE. This is where
  /// the real work takes place (most of it takes place in the time
  /// slab or even in the local elements), whereas the responsibility
  /// of the ODE solver is also to solve the dual problem (using this
  /// class), compute stability factors and compute error estimates.
  ///
  /// This class can be used in two different ways. One way is to
  /// call the static method solve() to solve a given ODE:
  ///
  ///   TimeStepper::solve(ode, u);
  ///
  /// Alternatively, one can create a TimeStepper object and use this
  /// for repeatedly time stepping the ODE, one time slab at a time:
  ///
  ///   TimeStepper timeStepper(ode, u);
  ///   timeStepper.step();
  ///   timeStepper.step();

  class TimeStepper
  {
  public:

    /// Constructor
    TimeStepper(ODE& ode, ODESolution& s);

    /// Destructor
    ~TimeStepper();
    
    /// Solve given ODE and store solution in ODESolution object
    static void solve(ODE& ode, ODESolution& s);

    /// Step solution, return current time
    real step();

    /// Check if we have reached the end time
    bool finished() const;

    /// Check if we have stopped
    bool stopped() const;

  private:

    // Save interpolated solution (when necessary)
    void save();

    // Save at fixed sample points
    void saveFixedSamples();
    
    // Save using adaptive samples
    void saveAdaptiveSamples();
    
    //--- Time-stepping data ---

    // Size of system
    unsigned int N;

    // Current time
    real t;

    // End time of computation
    real T;

    // The ODE being solved
    ODE& ode;

    ODESolution& solution;

    // The time slab
    TimeSlab* timeslab;

    // Storing the computed solution
    File file;

    // Progress bar
    Progress p;

    // True if solution has been stopped
    bool _stopped;

    // True if we have reached the given end time
    bool _finished;

    // True if we should save the solution
    bool save_solution;

    // True if we should use adaptive samples
    bool adaptive_samples;

    // Number of samples to save (for non-adaptive sampling)
    unsigned int no_samples;
    
    // Density of sampling (for adaptive sampling)
    unsigned int sample_density;

  };

}

#endif
