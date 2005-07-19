// Copyright (C) 2004-2005 Johan Jansson.
// Licensed under the GNU GPL Version 2.
//
// First added:  2004
// Last changed: 2005

#include <dolfin.h>

using namespace dolfin;

// Density
class Density : public Function
{
  real operator() (const Point& p) const
  {
    if(p.x < 0.01 && p.y > 0.0)
      return 1.0e3;
    else
      return 1.0e3;
  }
};

// Right-hand side
class Source : public Function
{
  real operator() (const Point& p, unsigned int i) const
  {
    if(i == 1)
      return 0.0;
    else
      return 0.0;
  }
};

// Initial velocity
class InitialVelocity : public Function
{
  real operator() (const Point& p, unsigned int i) const
  {
    //    if(i == 0)
    if(i == 1 && p.x > 0.5 )
    {
      return -0.01;
    }
    else if(i == 1 && p.x <= 0.5 && p.x > -0.5)
    {
      return 0.4;
    }
    else if(i == 1 && p.x <= -0.5)
    {
      return -0.4;
    }
    else
    {
      return 0.0;
    }

//     if(i == 1 && p.x > 0.0 )
//     {
//       return 0.8;
//     }

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

    if(p.x == 0.0)
    {
      value.set(0.0);
    }    

    return value;
  }
};

int main(int argc, char **argv)
{
  dolfin_output("text");

  //Mesh mesh("cow01.xml.gz");
  Mesh mesh("cow05.xml.gz");
  //Mesh mesh("mymesh01.xml.gz");
  //Mesh mesh("minimal2.xml.gz");

//   File outfile("mymesh.xml.gz");

//   for (NodeIterator n(&mesh); !n.end();)
//   {
//     Node &cnode = *n;
//     ++n;

//     if(cnode.coord().y > 10.0)
//     {
//       mesh.remove(cnode);
//     }
//   }

//   mesh.init();

//   outfile << mesh;
  
  Source f;
  Density rho;
  InitialVelocity v0;
  MyBC bc;

//   real T = 0.1;  // final time
//   real k = 0.001; // time step

  real T = 5.0;  // final time
  real k = 1.0e-2; // time step

  real E = 1.0e4; // Young's modulus
  real nu = 0.3; // Poisson's ratio
  real nuv = 4.0e3; // viscosity

  ElasticityUpdatedSolver::solve(mesh, f, v0, rho, E, nu, nuv, bc, k, T);

  return 0;
}
