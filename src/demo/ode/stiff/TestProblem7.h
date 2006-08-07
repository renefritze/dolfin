// Copyright (C) 2003-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003
// Last changed: 2006-07-05

#include <dolfin.h>

using namespace dolfin;

class TestProblem7 : public ODE
{
public:
  
  TestProblem7() : ODE(101, 1.0)
  {
    h = 1.0 / (static_cast<real>(N) - 1);
    dolfin_info("The heat equation on [0,1] with h = %f", h);
  }
  
  real u0(unsigned int i)
  {
    return 0.0;
  }

  void f(const uBlasVector& u, real t, uBlasVector& y)
  {
    // Boundary values
    y(0)   = 0.0;
    y(N-1) = 0.0;

    // Interior values
    for (unsigned int i = 1; i < N - 1; i++)
    {
      // Heat source
      real source = 0.0;
      if ( i == N/2 )
	source = 100.0;
      
      y[i] = (u[i-1] - 2.0*u[i] + u[i+1]) / (h*h) + source;
    }
  }
  
private:
  
  real h;

};
