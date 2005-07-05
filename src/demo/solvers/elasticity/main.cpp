// Copyright (C) 2004-2005 Johan Jansson.
// Licensed under the GNU GPL Version 2.
//
// First added:  2004-02-26
// Last changed: 2005

#include <dolfin.h>

using namespace dolfin;

// Right-hand side
class Source : public Function
{
  real operator() (const Point& p, unsigned int i) const
  {
//     if(i == 0)
//     {
//       if(time() >= 0.0 && time() < 0.5 && p.x > 0.99)
//       {
// 	return -20.0;
//       }
//       else if(time() >= 0.0 && time() < 0.5 && p.x < 0.01)
//       {
// 	return 20.0;
//       }
//     }
//     else
//     {
//       return 0.0;
//     }
//     return 0.0;
    if(i == 1)
      return -2.0;
    else
      return 0.0;
  }
};

// Initial displacement
class InitialDisplacement : public Function
{
  real operator() (const Point& p, unsigned int i) const
  {
    return 0.0;
  }
};

// Initial displacement
class InitialVelocity : public Function
{
  real operator() (const Point& p, unsigned int i) const
  {
    if(i == 1 && p.x > 0.0 )
      return 1.0;
    else
      return 0.0;
  }
};

// Boundary condition
class MyBC : public BoundaryCondition
{
public:
  MyBC::MyBC() : BoundaryCondition(3)
  {
  }

  const BoundaryValue operator() (const Point& p, int i)
  {
    BoundaryValue value;

    if ( p.x == 0.0 )
    {
      value.set(0.0);
    }    

    return value;
  }
};

int main(int argc, char **argv)
{
  dolfin_output("text");

  Mesh mesh("tetmesh-4.xml.gz");

  Source f;
  InitialDisplacement u0;
  InitialVelocity v0;
  MyBC bc;

  real T = 5.0;  // final time
  real k = 0.01; // time step

  real E = 10.0; // Young's modulus
  real nu = 0.3; // Poisson's ratio

  ElasticitySolver::solve(mesh, f, u0, v0, E, nu, bc, k, T);

  return 0;
}
