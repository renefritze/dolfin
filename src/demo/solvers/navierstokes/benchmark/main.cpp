// Copyright (C) 2005 Johan Hoffman 
// Licensed under the GNU GPL Version 2.
//
// A cG(1)cG(1) solver for the Navier-Stokes equations in 3D 
//
//     du/dt + u * grad u - nu * div grad u + grad p = f 
//     div u = 0 
//

#include <dolfin/NSESolver.h>

using namespace dolfin;

// Force term
class ForceFunction : public NewFunction
{
  real operator() (const Point& p, int i) const
  {
    if (i==0) return 0.0;
    if (i==1) return 0.0;
    if (i==2) return 0.0;
    dolfin_error("Wrong vector component index");
    return 0.0;
  }
};

// Initial solution 
class InitialSolution : public NewFunction
{
  real operator() (const Point& p, int i) const
  {
    if (i==0){
      if (p.y < 0.2) 
	return 5.0*p.y;
      else           
	return 1.0;
    }
    if (i==1){
      if ( (p.y < 0.2) && (fabs(p.z-0.5) < 0.25) )
	return - 0.05 * ( - cos(2.0*2.0*DOLFIN_PI*p.z) * sin(5.0*DOLFIN_PI*p.y) );
      else 
	return 0.0;
    }  
    if (i==2){
      if ( (p.y < 0.2) && (fabs(p.z-0.5) < 0.25) )
	return - 0.05 * ( - sin(2.0*2.0*DOLFIN_PI*p.z) * cos(5.0*DOLFIN_PI*p.y) );
      else 
	return 0.0;
    }
    dolfin_error("Wrong vector component index");
    return 0.0;
  }
};

// Boundary condition for momentum equation 
class BC_Momentum : public NewBoundaryCondition
{
  const BoundaryValue operator() (const Point& p, int i)
  {
    BoundaryValue value;
    if (i==0){
      if (fabs(p.x - 0.0) < DOLFIN_EPS){
	if (p.y < 0.2)
	  value.set(5.0*p.y);
	else 
	  value.set(1.0);
      }
      if (fabs(p.y - 0.0) < DOLFIN_EPS)  
	value.set(0.0);
    } else if (i==1){
      if (fabs(p.x - 0.0) < DOLFIN_EPS){
	if ( (p.y < 0.2) && (fabs(p.z-0.5) < 0.25) )
	  value.set(- 0.05 * ( - cos(2.0*2.0*DOLFIN_PI*p.z) * sin(5.0*DOLFIN_PI*p.y) ));
	else 
	  value.set(0.0);
      }
      if ( (fabs(p.y - 0.0) < DOLFIN_EPS) || (fabs(p.y - 1.0) < DOLFIN_EPS) ) 
	value.set(0.0);
    } else if (i==2){
      if (fabs(p.x - 0.0) < DOLFIN_EPS){
	if ( (p.y < 0.2) && (fabs(p.z-0.5) < 0.25) )
	  value.set(- 0.05 * ( - sin(2.0*2.0*DOLFIN_PI*p.z) * cos(5.0*DOLFIN_PI*p.y) ));
	else 
	  value.set(0.0);
      }
      if ( (fabs(p.z - 0.0) < DOLFIN_EPS) || (fabs(p.z - 1.0) < DOLFIN_EPS) ) 
	value.set(0.0);
    } else{
      dolfin_error("Wrong vector component index");
    }
  
    return value;
  }
};

// Boundary condition for continuity equation 
class BC_Continuity : public NewBoundaryCondition
{
  const BoundaryValue operator() (const Point& p)
  {
    BoundaryValue value;
    if (fabs(p.x - 12.0) < DOLFIN_EPS)
      value.set(0.0);
    
    return value;
  }
};

int main()
{
  Mesh mesh("mesh.xml.gz");
  ForceFunction f;
  InitialSolution u0; 
  BC_Momentum bc_mom;
  BC_Continuity bc_con;
  
  // Set parameters: T0, T, nu,...

  //NSESolver::solve(mesh, f, bc_mom, bc_con, u0); 
  
  return 0;
}
