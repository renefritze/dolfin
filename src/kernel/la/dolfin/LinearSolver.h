// Copyright (C) 2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __LINEAR_SOLVER_H
#define __LINEAR_SOLVER_H

#include <dolfin/Vector.h>
#include <dolfin/Matrix.h>
#include <dolfin/VirtualMatrix.h>

namespace dolfin
{

  /// This class defines the interface of all linear solvers for
  /// systems of the form Ax = b.
  
  class LinearSolver
  {
  public:

    /// Constructor
    LinearSolver();

    /// Destructor
    virtual ~LinearSolver();

    /// Solve linear system Ax = b
    virtual void solve(const Matrix& A, Vector& x, const Vector& b) = 0;
    
    /// Solve linear system Ax = b (matrix-free version)
    virtual void solve(const VirtualMatrix& A, Vector& x, const Vector& b) = 0;

  };

}

#endif
