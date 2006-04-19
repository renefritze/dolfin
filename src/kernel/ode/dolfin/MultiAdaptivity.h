// Copyright (C) 2005-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-01-29
// Last changed: 2006-04-19

#ifndef __MULTI_ADAPTIVITY_H
#define __MULTI_ADAPTIVITY_H

#include <dolfin/constants.h>
#include <dolfin/Controller.h>
#include <dolfin/Adaptivity.h>

namespace dolfin
{
  class ODE;
  class Method;

  /// This class controls the multi-adaptive time-stepping

  class MultiAdaptivity : public Adaptivity
  {
  public:

    /// Constructor
    MultiAdaptivity(const ODE& ode, const Method& method);

    /// Destructor
    ~MultiAdaptivity();

    /// Return time step for given component
    real timestep(uint i) const;

    /// Start time step update for system
    void updateStart();

    /// Update time step for given component
    void updateComponent(uint i, real k0, real r, real rmax,
			 real error, const Method& method, real t);
    
    /// End time step update for system
    void updateEnd(bool first);

  private:

    // Multi-adaptive time steps
    real* timesteps;

  };

}

#endif
