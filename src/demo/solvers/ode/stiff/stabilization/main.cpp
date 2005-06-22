// Copyright (C) 2004 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

// Test problems for the adaptive stabilization of stiff problems over
// one time step. The test problems are different versions of the
// nonnormal test problem. Typical way of running obtaining data from
// the iteration:
//
//   ./dolfin-ode-stiff-test | grep debug1 | cut -d':' -f2 > iter
//
// which may not be very nice, but this is only for debugging... :)
// The number printed are the residual, increment, and damping.

#include <dolfin.h>

using namespace dolfin;

class TestProblem : public ODE
{
public:
  
  TestProblem(real spring) : ODE(2), A(2,2)
  {
    T = 1.0;

    A(0,0) = 0.0;    
    A(0,1) = - 1.0;
    A(1,0) = spring;
    A(1,1) = 200.0;
    
    A.disp();
  }

  real u0(unsigned int i)
  {
    return 1.0;
  }

  real f(const Vector& u, real t, unsigned int i)
  {
    // FIXME: BROKEN
    dolfin_error("Broken, needs to be updated.");
    //return -A.mult(u, i);
    return 0.0;
  }

private:
  
  Matrix A;
  
};

int main()
{
  dolfin_set("tolerance", 1e-12);
  dolfin_set("method", "dg");
  dolfin_set("order", 0);
  dolfin_set("initial time step", 1.0);
  dolfin_set("maximum time step", 1.0);
  dolfin_set("fixed time step", true);
  dolfin_set("solve dual problem", false);
  dolfin_set("stiffness", "stiff level 3");
  dolfin_set("maximum iterations", 1000);
  dolfin_set("debug iterations", true);

  // Critical damping
  TestProblem test1(1e4);
  test1.solve();

  // Underdamped
  TestProblem test2(2e4);
  test2.solve();

  // Overdamped
  TestProblem test3(1e3);
  test3.solve();
  
  return 0;
}
