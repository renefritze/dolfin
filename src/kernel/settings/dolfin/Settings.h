// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __SETTINGS_H
#define __SETTINGS_H

#include <dolfin/constants.h>
#include <dolfin/ParameterList.h>

namespace dolfin {
  
  /// Pre-defined global parameters within DOLFIN.

  class Settings : public ParameterList {
  public:
    
    Settings() : ParameterList() {
      
      // General parameters

      add(Parameter::REAL, "tolerance", 0.1);
      add(Parameter::REAL, "start time", 0.0);
      add(Parameter::REAL, "end time",   10.0);
      add(Parameter::REAL, "krylov tolerance", 1.0e-10);
      add(Parameter::REAL, "progress step", 0.1);

      add(Parameter::INT, "max no krylov restarts", 100);
      add(Parameter::INT, "max no stored krylov vectors", 100);
      add(Parameter::INT, "max no cg iterations", 1000);
      add(Parameter::INT, "pc iterations", 5);

      add(Parameter::STRING, "output", "curses");
      add(Parameter::STRING, "temporary directory", "/tmp");

      add(Parameter::BCFUNCTION, "boundary condition", 0);
      
      add(Parameter::BOOL, "save each mesh", false);

      // Parameters for multi-adaptive solver

      add(Parameter::BOOL, "fixed time step", false);
      add(Parameter::BOOL, "solve dual problem", true);
      add(Parameter::BOOL, "save solution", true);
      add(Parameter::BOOL, "adaptive samples", false);
      add(Parameter::BOOL, "debug time steps", false);
      add(Parameter::BOOL, "debug iterations", false);
      add(Parameter::BOOL, "automatic modeling", false);

      add(Parameter::INT, "number of samples", 100);
      add(Parameter::INT, "sample density", 1);
      add(Parameter::INT, "element cache size", 32);
      add(Parameter::INT, "maximum iterations", 100);
      add(Parameter::INT, "order", 1);
      add(Parameter::INT, "average samples", 1000);
      
      add(Parameter::REAL, "initial time step", 0.01);
      add(Parameter::REAL, "maximum time step", 0.1);
      add(Parameter::REAL, "partitioning threshold", 0.5);
      add(Parameter::REAL, "interval threshold", 0.9);
      add(Parameter::REAL, "sparsity check increment", 0.01);
      add(Parameter::REAL, "maximum divergence", 10.0);
      add(Parameter::REAL, "maximum convergence", 0.75);
      add(Parameter::REAL, "average length", 0.1);
      add(Parameter::REAL, "average tolerance", 0.1);
      
      add(Parameter::STRING, "method", "cg");
      add(Parameter::STRING, "stiffness", "non-stiff");

      // Parameters for multgrid solver

      add(Parameter::REAL, "multigrid tolerance", 0.001);
      add(Parameter::INT,  "multigrid pre-smoothing", 9);
      add(Parameter::INT,  "multigrid post-smoothing", 9);
      add(Parameter::INT,  "multigrid iterations", 100);

     }
    
  };
  
}

#endif
