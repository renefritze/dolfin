// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin.h>

using namespace dolfin;

class Lorenz : public ODE {
public:

  Lorenz() : ODE(3)
  {
    // Parameters
    s = 10.0;
    b = 8.0 / 3.0;
    r = 28.0;
    
    // Final time
    T = 50.0;
  }

  real u0(unsigned int i)
  {
    switch (i) {
    case 0:
      return 1.0;
    case 1:
      return 0.0;
    default:
      return 0.0;
    }
  }

  real f(real u[], real t, unsigned int i)
  {
    switch (i) {
    case 0:
      return s*(u[1] - u[0]);
    case 1:
      return r*u[0] - u[1] - u[0]*u[2];
    default:
      return u[0]*u[1] - b*u[2];
    }
  }

  private:

    real s;
    real b;
    real r;

};

int main()
{
  dolfin_set("output", "plain text");
  dolfin_set("number of samples", 500);
  dolfin_set("solve dual problem", false);
  dolfin_set("initial time step", 0.05);
  dolfin_set("fixed time step", true);
  dolfin_set("use new ode solver", true);
  dolfin_set("method", "cg");
  dolfin_set("order", 10);
  dolfin_set("tolerance", 1e-10);
  //dolfin_set("save solution", false);

  Lorenz lorenz;
  lorenz.solve();
  
  return 0;
}
