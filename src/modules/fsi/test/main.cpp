// Copyright (C) 2005 Johan Hoffman.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg 2005.
//
// First added:  2002-11-29
// Last changed: 2005-12-28
//
// A cG(1)cG(1) solver for the incompressible Navier-Stokes equations 
//
//     du/dt + u * grad u - nu * div grad u + grad p = f 
//     div u = 0 

#include <dolfin.h>
#include <dolfin/dolfin_modules.h>

using namespace dolfin;





//---------------------------------------------------------------------
// Force term
class ForceFunction_2D : public Function
{
  real eval(const Point& p, unsigned int i)
  {
    if (i==0) return 0.0;
    if (i==1) return 0.0;
    dolfin_error("Wrong vector component index");
    return 0.0;
  }
};
//---------------------------------------------------------------------
// Boundary condition for momentum equation 
class BC_Momentum_2D : public ALEBoundaryCondition
{
  void eval(BoundaryValue& value, const Point& p, const Point& r, unsigned int i)
  {
     real bmarg = 1.0e-3;

     if (i==0) {
       if (r.y() < 0.5) {   //fluid

	 if (r.x() < (2.0 - DOLFIN_EPS - bmarg))
	   value.set(0.0);

	 if (r.x() < (0.0 + DOLFIN_EPS + bmarg))
	   value.set((0.5 - r.y()) * r.y() * 20);

       } else {            //structure
	 value.set(0.0);
       }
     } else if (i==1) {
       
       value.set(0.0);
     }
     
  }
};
//---------------------------------------------------------------------
// Boundary condition for continuity equation 
class BC_Continuity_2D : public ALEBoundaryCondition
{
  void eval(BoundaryValue& value, const Point& p, const Point& r, unsigned int i)
  {
    real bmarg = 1.0e-3;

    if (r.y() < 0.5)   //fluid
      if (p.x() > (2.0 - DOLFIN_EPS - bmarg))
	value.set(0.0);
  }
};
//---------------------------------------------------------------------
class BisectionFunction : public Function
{
  real eval(const Point& p, unsigned int i)
  {
    if (p.y() > (0.5)) return 0; // structure
    return 1;                                 // fluid
  }
};
//---------------------------------------------------------------------
int main(int argc, char* argv[])
{
  dolfin_init(argc, argv);

  // mesh rectangle [0,0] - [2,1]
  Mesh mesh("rect_ale_ns.xml");
 
  //need to refine mesh
  for (int grain = 0; grain < 4; grain++) 
    mesh.refine();
  
  BC_Momentum_2D    bc_mom;
  BC_Continuity_2D  bc_con;
  ForceFunction_2D  f;
  BisectionFunction bisect;
 
  real rhof = 1;
  real rhos = 1;
  real k    = 1e-2;
  real E    = 20;
  real elnu = 0.3;
  real nu   = 1.0/3900.0; // fluid:     viscosity
  real T    = 2.0;        // final time
  
  
  FSISolver::solve(mesh, f, bc_mom, bc_con, bisect, rhof, rhos, E, elnu, nu, T, k); 
  return 0;
}
