// Copyright (C) 2008 Anders Logg
// Licensed under the GNU LGPL Version 2.1.
//
// First added: 2008-06-11
// Last change: 2009-02-13
//
// This demo solves the harmonic oscillator on
// the time interval (0, 4*pi) including the dual problem

#include <dolfin.h>

using namespace dolfin;

class Harmonic : public ODE
{
public:
  
  Harmonic() : ODE(2, 4.0 * DOLFIN_PI), e(0.0) {}

  void u0(real* u)
  {
    u[0] = 0.0;
    u[1] = 1.0;
  }

  void f(const real* u, real t, real* y)
  {
    y[0] = u[1];
    y[1] = - u[0];
  }

  bool update(const real* u, real t, bool end)
  {
    if (!end)
      return true;

    real e0 = u[0] - 0.0;
    real e1 = u[1] - 1.0;
    e = max(abs(e0), abs(e1));

    return true;
  }
  
  real error()
  {
    return e;
  }
  
private:

  real e;

};

int main()
{
  dolfin_set("ODE fixed time step", true);
  dolfin_set("ODE discrete tolerance", 1e-14);

  dolfin_set("ODE method", "cg");
  dolfin_set("ODE order", 10);
  dolfin_set("ODE solve dual problem", true);

  Harmonic ode;
  ode.solve();
}
