// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __SOLVER_POISSON_H
#define __SOLVER_POISSON_H

#include <Solver.h>

namespace dolfin {
  
  class SolverPoisson : public Solver {
  public:
	 
	 SolverPoisson(Grid *grid) : Solver(grid) {}
	 ~SolverPoisson(){}
	 
	 const char *Description();
	 void solve();
    
  };

}

#endif
