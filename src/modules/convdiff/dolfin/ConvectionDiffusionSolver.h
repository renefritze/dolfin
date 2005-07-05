// Copyright (C) 2003-2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003
// Last changed: 2005

#ifndef __CONVECTION_DIFFUSION_SOLVER_H
#define __CONVECTION_DIFFUSION_SOLVER_H

#include <dolfin/Solver.h>

namespace dolfin
{

  class ConvectionDiffusionSolver : public Solver
  {
  public:
    
    // Create convection-diffusion solver
    ConvectionDiffusionSolver(Mesh& mesh, Function& w, Function& f, BoundaryCondition& bc);
    
    // Solve convection-diffusion
    void solve();
    
    // Solve convection-diffusion (static version)
    static void solve(Mesh& mesh, Function& w, Function& f, BoundaryCondition& bc);
  
  private:

    Mesh& mesh;
    Function& w;
    Function& f;
    BoundaryCondition& bc;

  };

}

#endif
