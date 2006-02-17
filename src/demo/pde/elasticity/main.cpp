// Copyright (C) 2006 Johan Jansson and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-02-07
// Last changed: 2006-02-07
//
// This demo program solves the equations of static
// linear elasticity for a gear clamped at two of its
// ends and twisted 30 degrees.

#include <dolfin.h>
#include "Elasticity.h"
#include "ElasticityStrain.h"

using namespace dolfin;

int main()
{
  // Boundary condition
  class : public BoundaryCondition
  {
    void eval(BoundaryValue& value, const Point& p, unsigned int i)
    {
      // Width of clamp
      real w = 0.1;
      
      // Center of rotation
      real y0 = 0.5;
      real z0 = 0.219;
      
      // Angle of rotation (30 degrees)
      real theta = 0.5236;
      
      // New coordinates
      real y = y0 + (p.y - y0)*cos(theta) - (p.z - z0)*sin(theta);
      real z = z0 + (p.y - y0)*sin(theta) + (p.z - z0)*cos(theta);
      
      // Clamp at left end
      if ( p.x < w )
	value = 0.0;
      
      // Clamp at right end
      if ( p.x > (1.0 - w) )
      {
	if ( i == 1 )
	  value = y - p.y;
	else if ( i == 2 )
	  value = z - p.z;
      }
    }
  } bc;

  // Set up problem
  Mesh mesh("gear.xml.gz");
  Function f = 0.0;
  Elasticity::BilinearForm a;
  Elasticity::LinearForm L(f);
  PDE pde(a, L, mesh, bc);

  // Compute solution
  Function u = pde.solve();

  // Save solution (displacement) to file
  File file("elasticity.pvd");
  file << u;

  // Set up post-processing problem to compute strain
  ElasticityStrain::BilinearForm a_strain;
  ElasticityStrain::LinearForm L_strain(u);
  PDE pde_strain(a_strain, L_strain, mesh);
  Function normal_strain, shear_strain;

  // Compute solution
   pde_strain.solve(normal_strain, shear_strain);

  // Save solution (strain) to files
  File file_normal_strain("normal_strain.pvd");
  File file_shear_strain("shear_strain.pvd");
  file_normal_strain << normal_strain;
  file_shear_strain  << shear_strain;

  return 0;
}
