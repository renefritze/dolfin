// Copyright (C) 2004, 2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin.h>

using namespace dolfin;

class TestProblem1 : public ODE
{
public:
  
  TestProblem1() : ODE(1)
  {
    dolfin_info("The simple test equation: u' = -1000 u, u(0) = 1.");

    // Final time
    T = 10;
  }

  real u0(unsigned int i)
  {
    return 1.0;
  }
  
  real f(const real u[], real t, unsigned int i)
  {
    return -1000.0 * u[0];
  }
  
};
