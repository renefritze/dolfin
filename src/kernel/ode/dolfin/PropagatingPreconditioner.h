// Copyright (C) 2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __PROPAGATING_PRECONDITIONER_H
#define __PROPAGATING_PRECONDITIONER_H

#include <dolfin/NewPreconditioner.h>

namespace dolfin
{

  class NewVector;
  class NewJacobianMatrix;
  class ODE;
  class NewTimeSlab;
  class NewMethod;
  
  /// This class implements a preconditioner for the Newton system to
  /// be solved on each time slab. The preconditioner does
  /// Gauss-Seidel type fixed point iteration forward in time using
  /// diagonally scaled dG(0), and is responsible for propagating the
  /// values forward in time in each GMRES iteration.

  class PropagatingPreconditioner : public NewPreconditioner
  {
  public:

    /// Constructor
    PropagatingPreconditioner(const NewJacobianMatrix& A);

    /// Destructor
    ~PropagatingPreconditioner();
    
    /// Solve linear system approximately for given right-hand side b
    void solve(NewVector& x, const NewVector& b) const;

  private:

    /// The Jacobian of the time slab system
    const NewJacobianMatrix& A;

    // The ODE
    ODE& ode;

    // The time slab
    NewTimeSlab& ts;
    
    // Method, mcG(q) or mdG(q)
    const NewMethod& method;

  };

}

#endif
