// Copyright (C) 2008-2010 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2009-2011.
//
// First added:  2008-08-26
// Last changed: 2011-03-24
// Last changed: 2010-07-16

#ifndef __GENERIC_LINEAR_SOLVER_H
#define __GENERIC_LINEAR_SOLVER_H

#include <dolfin/common/Variable.h>
#include <dolfin/log/log.h>

namespace dolfin
{

  // Forward declarations
  class GenericMatrix;
  class GenericVector;

  /// This class provides a general solver for linear systems Ax = b.

  class GenericLinearSolver : public Variable
  {
  public:

    /// Set the operator (matrix)
    virtual void set_operator(const GenericMatrix& A) = 0;

    /// Set the operator (matrix) and preconditioner matrix
    virtual void set_operators(const GenericMatrix& A, const GenericMatrix& P)
    { error("Linear algebra backend solver does not support 'set_operators'."); }

    /// Get the operator (matrix)
    virtual const GenericMatrix& get_operator() const = 0;

    /// Solve linear system Ax = b
    virtual uint solve(const GenericMatrix& A, GenericVector& x, const GenericVector& b)
    { error("solve(A, x, b) is not implemented. Consider trying solve(x, b)."); return 0; }

    /// Solve linear system Ax = b
    virtual uint solve(GenericVector& x, const GenericVector& b)
    { error("solve(x, b) is not yet implemented for this backend."); return 0; }

  protected:

    // Check dimensions of matrix and vector(s)
    void check_dimensions(const GenericMatrix& A,
                          GenericVector& x,
                          const GenericVector& b) const;

  };

}

#endif
