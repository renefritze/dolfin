// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __MULTIGRID_SOLVER_H
#define __MULTIGRID_SOLVER_H

namespace dolfin {
  
  class GridHierarchy;
  class Matrix;
  class Vector;
  
  class MultigridSolver{
  public:
    
    MultigridSolver(GridHierarchy& grids);
    MultigridSolver();
    ~MultigridSolver();
    
    /// Solve Ax = b 
    void solve(const Matrix& A, Vector& x, const Vector& b);

  private:
    
    GridHierarchy& grids;

  };
  
}
  
#endif
