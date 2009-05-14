// Copyright (C) 2004-2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2004
// Last changed: 2006-08-21

#include <cmath>
#include <dolfin.h>

using namespace dolfin;

class TestProblem5 : public ODE
{
public:

  TestProblem5() : ODE(6, 180.0)
  {
    info("The Chemical Akzo-Nobel problem.");

    k1  = 18.7;
    k2  = 0.58;
    k3  = 0.09;
    k4  = 0.42;
    K   = 34.4;
    klA = 3.3;
    Ks  = 115.83;
    p   = 0.9;
    H   = 737.0;
  }

  void u0(double* u)
  {
    u[0] = 0.444;
    u[1] = 0.00123;
    u[2] = 0.0;
    u[3] = 0.007;
    u[4] = 0.0;
    u[5] = 0.36;
  }

  void f(const double* u, double t, double* y)
  {
    y[0] = -2.0*r1(u) + r2(u) - r3(u) - r4(u);
    y[1] = -0.5*r1(u) - r4(u) - 0.5*r5(u) + F(u);
    y[2] = r1(u) - r2(u) + r3(u);
    y[3] = -r2(u) + r3(u) - 2.0*r4(u);
    y[4] = r2(u) - r3(u) + r5(u);
    y[5] = Ks*u[0]*u[3] - u[5];
  }

private:

  double r1(const double* u)
  {
    return k1*dolfin::pow(u[0], 4.0)*sqrt(u[1]);
  }

  double r2(const double* u)
  {
    return k2*u[2]*u[3];
  }

  double r3(const double* u)
  {
    return (k2/K)*u[0]*u[4];
  }

  double r4(const double* u)
  {
    return k3*u[0]*dolfin::pow(u[3], 2.0);
  }

  double r5(const double* u)
  {
    return k4*dolfin::pow(u[5], 2.0)*sqrt(u[1]);
  }

  double F(const double* u)
  {
    return klA * (p/H - u[1]);
  }

  double k1, k2, k3, k4, K, klA, Ks, p, H;

};
