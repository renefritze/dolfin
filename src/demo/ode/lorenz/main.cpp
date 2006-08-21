// Copyright (C) 2003-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003-07-02
// Last changed: 2006-08-21

#include <stdio.h>
#include <dolfin.h>

using namespace dolfin;

class Lorenz : public ODE
{
public:

  Lorenz() : ODE(3, 500.0)
  {
    // Parameters
    s = 10.0;
    b = 8.0 / 3.0;
    r = 28.0;

    // Fixed points
    p0.x = 6.0*sqrt(2.0); p0.y = p0.x; p0.z = 27.0;
    p1.x = -p0.x; p1.y = -p0.y; p1.z = p0.z;

    // Distance between fixed points
    v = p1 - p0;
    
    // Current fixed point
    pos = -1;

    // Number of laps around the two fixed points
    n0 = 0;
    n1 = 0;

    // Open file
    fp = fopen("lorenz.data", "w");
  }
  
  ~Lorenz()
  {
    fclose(fp);
  }

  void u0(uBlasVector& u)
  {
    u(0) = 1.0;
    u(1) = 0.0;
    u(2) = 0.0;
  }

  void f(const uBlasVector& u, real t, uBlasVector& y)
  {
    y(0) = s*(u(1) - u(0));
    y(1) = r*u(0) - u(1) - u(0)*u(2);
    y(2) = u(0)*u(1) - b*u(2);
  }

  void J(const uBlasVector& x, uBlasVector& y, const uBlasVector& u, real t)
  {
    y(0) = s*(x(1) - x(0));
    y(1) = (r - u(2))*x(0) - x(1) - u(0)*x(2);
    y(2) = u(1)*x(0) + u(0)*x(1) - b*x(2);
  }

  bool update(const uBlasVector& u, real t, bool end)
  {
    // Check in which region the point is
    Point p(u(0), u(1), u(2));

    if ( (p - p0) * v < 0 )
    {
      if ( pos != 0 )
      {
	n0++;
	pos = 0;
	real alpha = 0.0;
	if ( n0 > 0 & n1 > 0 )
	  alpha = static_cast<real>(n0) / (static_cast<real>(n1));
	fprintf(fp, "%.12e 0 %d %d %.16e\n", t, n0, n1, alpha);
      }
    }
    else if ( (p - p1) * v > 0 )
    {
      if ( pos != 1 )
      {
	n1++;
	pos = 1;
	real alpha = 0.0;
	if ( n0 > 0 & n1 > 0 )
	  alpha = static_cast<real>(n0) / (static_cast<real>(n1) + DOLFIN_EPS);
	fprintf(fp, "%.12e 1 %d %d %.16e\n", t, n0, n1, alpha);
      }
    }
    else
      pos = -1;

    return true;
  }

private:

  // Parameters
  real s;
  real b;
  real r;

  // The two fixed points (not counting x = (0, 0, 0))
  Point p0;
  Point p1;

  // Vector p1 - p0
  Point v;
  
  // Current fixed point
  int pos;

  // Number of laps around the two fixed points
  int n0;
  int n1;

  // File pointer
  FILE* fp;

};

int main()
{
  dolfin_output("plain text");

  set("ODE number of samples", 500);
  set("ODE initial time step", 0.01);
  set("ODE fixed time step", true);
  set("ODE nonlinear solver", "newton");
  set("ODE method", "cg");
  set("ODE order", 5);
  set("ODE discrete tolerance", 1e-12);
  set("ODE save solution", true);
 
  Lorenz lorenz;
  lorenz.solve();
  
  return 0;
}
