// Copyright (C) 2003-2005 Johan Jansson.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg 2003-2006.
//
// First added:  2003
// Last changed: 2006-08-21

#include <dolfin.h>

using namespace dolfin;

class TestProblem8 : public ODE
{
public:

  TestProblem8() : ODE(3, 0.3)
  {
    info("System of fast and slow chemical reactions, taken from the book by");
    info("Hairer and Wanner, page 3.");
  }

  void u0(Array<real>& u)
  {
    u[0] = 1.0;
    u[1] = 0.0;
    u[2] = 0.0;
  }

  void f(const Array<real>& u, real t, Array<real>& y)
  {
    y[0] = -0.04 * u[0] + 1.0e4 * u[1] * u[2];
    y[1] = 0.04 * u[0] - 1.0e4 * u[1] * u[2] - 3.0e7 * u[1] * u[1];
    y[2] = 3.0e7 * u[1] * u[1];
  }

};
