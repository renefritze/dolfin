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
    T = 10.0;

    // Initial value
    u0(0) = 1.0;
    u0(1) = 0.0;
    u0(2) = 0.0;

    // Sparsity (not really necessary here)
    sparse();
  }

  real f(const Vector& u, real t, int i)
  {
    switch (i) {
    case 0:
      return s*(u(1) - u(0));
    case 1:
      return r*u(0) - u(1) - u(0)*u(2);
    default:
      return u(0)*u(1) - b*u(2);
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

  Lorenz lorenz;
  lorenz.solve();
  
  return 0;
}
